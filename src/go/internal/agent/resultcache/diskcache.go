/*
** Zabbix
** Copyright (C) 2001-2020 Zabbix SIA
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
**/
package resultcache

import (
	"database/sql"
	"encoding/json"
	"fmt"
	"time"

	"zabbix.com/internal/agent"
	"zabbix.com/internal/monitor"
	"zabbix.com/pkg/itemutil"
	"zabbix.com/pkg/log"
	"zabbix.com/pkg/plugin"
	"zabbix.com/pkg/version"

	_ "github.com/mattn/go-sqlite3"
)

const (
	DbVariableNotSet = -1
)

type DiskCache struct {
	baseCache
	token         string
	lastDataID    uint64
	clientID      uint64
	lastError     error
	retry         *time.Timer
	timeout       int
	persistPeriod int
	oldestLog     uint64
	oldestData    uint64
	connectId     int
	database      *sql.DB
}

func (c *DiskCache) resultFetch(rows *sql.Rows) AgentData {
	var tmp uint64
	var data AgentData
	var LastLogSize int64
	var Mtime, State, EventID, EventSeverity, EventTimestamp int
	var Value, EventSource string

	rows.Scan(&data.Id, &data.Itemid, &LastLogSize, &Mtime, &State, &Value, &EventSource, &EventID,
		&EventSeverity, &EventTimestamp, &data.Clock, &data.Ns)
	if LastLogSize != DbVariableNotSet {
		tmp = uint64(LastLogSize)
		data.LastLogsize = &tmp
	}
	if Mtime != DbVariableNotSet {
		data.Mtime = &Mtime
	}
	if State != DbVariableNotSet {
		data.State = &State
	}
	if Value != "" {
		data.Value = &Value
	}
	if EventSource != "" {
		data.EventSource = &EventSource
	}
	if EventID != DbVariableNotSet {
		data.EventID = &EventID
	}
	if EventSeverity != DbVariableNotSet {
		data.EventSeverity = &EventSeverity
	}
	if EventTimestamp != DbVariableNotSet {
		data.EventTimestamp = &EventTimestamp
	}
	return data
}

func (c *DiskCache) upload(u Uploader) (err error) {

	var results []*AgentData
	var rows *sql.Rows

	if rows, err = c.database.Query(fmt.Sprintf("SELECT * FROM data_%d", c.connectId)); err != nil {
		log.Errf("[%d] cannot select from data table: %s", c.clientID, err.Error())
		return
	}
	for rows.Next() {
		result := c.resultFetch(rows)
		result.persistent = false
		results = append(results, &result)

	}
	if rows, err = c.database.Query(fmt.Sprintf("SELECT * FROM log_%d", c.connectId)); err != nil {
		log.Errf("[%d] cannot select from log table: %s", c.clientID, err.Error())
		return
	}
	for rows.Next() {
		result := c.resultFetch(rows)
		result.persistent = true
		results = append(results, &result)
	}
	if len(results) == 0 {
		return
	}

	request := AgentDataRequest{
		Request: "agent data",
		Data:    results,
		Session: c.token,
		Host:    agent.Options.Hostname,
		Version: version.Short(),
	}

	var data []byte

	if data, err = json.Marshal(&request); err != nil {
		log.Errf("[%d] cannot convert cached history to json: %s", c.clientID, err.Error())
		return
	}

	timeout := len(results) * c.timeout
	if timeout > 60 {
		timeout = 60
	}
	if err = u.Write(data, time.Duration(timeout)*time.Second); err != nil {
		if c.lastError == nil || err.Error() != c.lastError.Error() {
			log.Warningf("[%d] history upload to [%s] started to fail: %s", c.clientID, u.Addr(), err)
			c.lastError = err
		}
		return
	}

	if c.lastError != nil {
		log.Warningf("[%d] history upload to [%s] is working again", c.clientID, u.Addr())
		c.lastError = nil
	}
	c.database.Exec(fmt.Sprintf("DELETE FROM data_%d", c.connectId))
	c.database.Exec(fmt.Sprintf("DELETE FROM log_%d", c.connectId))
	c.oldestData = 0
	c.oldestLog = 0

	return
}

func (c *DiskCache) flushOutput(u Uploader) {
	if c.retry != nil {
		c.retry.Stop()
		c.retry = nil
	}

	if c.upload(u) != nil && u.CanRetry() {
		c.retry = time.AfterFunc(UploadRetryInterval, func() { c.Upload(u) })
	}
}

func (c *DiskCache) write(r *plugin.Result) {
	c.lastDataID++

	var LastLogsize int64 = DbVariableNotSet
	if r.LastLogsize != nil {
		LastLogsize = int64(*r.LastLogsize)
	}

	var Value string
	var State int = DbVariableNotSet
	if r.Error != nil {
		Value = r.Error.Error()
		State = itemutil.StateNotSupported
	} else if r.Value != nil {
		Value = *r.Value
	}

	var ns int
	var clock uint64
	if !r.Ts.IsZero() {
		clock = uint64(r.Ts.Unix())
		ns = r.Ts.Nanosecond()
	}

	var Mtime int = DbVariableNotSet
	if r.Mtime != nil {
		Mtime = *r.Mtime
	}

	var EventSource string
	if r.EventSource != nil {
		EventSource = *r.EventSource
	}

	var EventID int = DbVariableNotSet
	if r.EventID != nil {
		EventID = *r.EventID
	}

	var EventSeverity int = DbVariableNotSet
	if r.EventSeverity != nil {
		EventSeverity = *r.EventSeverity
	}

	var EventTimestamp int = DbVariableNotSet
	if r.EventTimestamp != nil {
		EventTimestamp = *r.EventTimestamp
	}

	var stmt *sql.Stmt

	if r.Persistent {
		if c.oldestLog == 0 {
			c.oldestLog = clock
		}
		if (clock - c.oldestLog) <= uint64(c.persistPeriod) {
			stmt, _ = c.database.Prepare(c.InsertResultTable(fmt.Sprintf("log_%d", c.connectId)))
		}
	} else {
		if c.oldestData == 0 {
			c.oldestData = clock
		}
		if (clock - c.oldestData) > uint64(c.persistPeriod) {
			query := fmt.Sprintf("DELETE FROM data_%d WHERE clock = %d", c.connectId, c.oldestData)
			c.database.Exec(query)
			rows, err := c.database.Query(fmt.Sprintf("SELECT MIN(Clock) FROM data_%d", c.connectId))
			if err != nil {
				for rows.Next() {
					rows.Scan(&c.oldestData)
				}
			}
		}
		stmt, _ = c.database.Prepare(c.InsertResultTable(fmt.Sprintf("data_%d", c.connectId)))
	}
	if stmt != nil {
		stmt.Exec(c.lastDataID, r.Itemid, LastLogsize, Mtime, State, Value,
			EventSource, EventID, EventSeverity, EventTimestamp, clock, ns)
	}
}

func (c *DiskCache) run() {
	defer log.PanicHook()
	log.Debugf("[%d] starting disk cache", c.clientID)

	for {
		u := c.Input()
		if u == nil {
			break
		}
		switch v := u.(type) {
		case Uploader:
			c.flushOutput(v)
		case *plugin.Result:
			c.write(v)
		case *agent.AgentOptions:
			c.updateOptions(v)
		}
	}
	log.Debugf("[%d] disk cache has been stopped", c.clientID)
	if c.database != nil {
		c.database.Close()
	}
	monitor.Unregister(monitor.Output)
}

func (c *DiskCache) updateOptions(options *agent.AgentOptions) {
	c.persistPeriod = options.PersistentBufferPeriod
}

func (c *DiskCache) InsertResultTable(table string) string {
	return fmt.Sprintf(`
		INSERT INTO %s
		(Id, Itemid, LastLogsize, Mtime, State, Value, EventSource, EventID, EventSeverity, EventTimestamp, Clock, Ns)
		VALUES
		(?,?,?,?,?,?,?,?,?,?,?,?)
	`, table)
}

func (c *DiskCache) init(u Uploader) {
	c.updateOptions(&agent.Options)
	c.InitBase(u)

	var err error
	c.database, err = sql.Open("sqlite3", agent.Options.PersistentBufferFile)
	if err != nil {
		return
	}

	var rows *sql.Rows
	rows, err = c.database.Query(fmt.Sprintf("SELECT Id FROM registry WHERE Address = '%s'", c.Uploader().Addr()))
	if err == nil {
		for rows.Next() {
			rows.Scan(&c.connectId)
		}
	}

	if rows, err = c.database.Query(fmt.Sprintf("SELECT MIN(Clock), MAX(Id) FROM data_%d", c.connectId)); err == nil {
		for rows.Next() {
			rows.Scan(&c.oldestData, &c.lastDataID)
		}
	}
}

func (c *DiskCache) Start() {
	// register with secondary group to stop result cache after other components are stopped
	monitor.Register(monitor.Output)
	go c.run()
}

func (c *DiskCache) SlotsAvailable() int {
	return int(^uint(0) >> 1) //Max int
}

func (c *DiskCache) PersistSlotsAvailable() int {
	return int(^uint(0) >> 1) //Max int
}
