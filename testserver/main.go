// Copyright 2015 The Gorilla WebSocket Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

//go:build ignore
// +build ignore

package main

import (
	"encoding/json"
	"flag"
	"fmt"
	"log"
	"math"
	"net/http"
	"time"

	"github.com/gorilla/websocket"
)

/*
{
	"type": "advertisement",
	"channels": [
		{
			"channelId": 0,
			"schema": {
				"name": "asdf",
				"type": "record",
				"fields": []
			}
		}
	]
}
*/

type Schema interface {
	MarshalJSON() ([]byte, error)
}

type StructSchema struct {
	Name   string   `json:"name"`
	Fields []Schema `json:"fields"`
}

func (s StructSchema) MarshalJSON() ([]byte, error) {
	type Wrapper struct {
		Name   string   `json:"name"`
		Type   string   `json:"type"`
		Fields []Schema `json:"fields"`
	}
	var w = Wrapper{s.Name, "record", s.Fields}
	return json.Marshal(w)
}

type DoubleSchema struct {
	Name string `json:"name"`
}

func (d DoubleSchema) MarshalJSON() ([]byte, error) {
	type Wrapper struct {
		Name string `json:"name"`
		Type string `json:"type"`
	}
	var w = Wrapper{d.Name, "float"}
	return json.Marshal(w)
}

type StringSchema struct {
	Name string
}

func (d StringSchema) MarshalJSON() ([]byte, error) {
	type Wrapper struct {
		Name string `json:"name"`
		Type string `json:"type"`
	}
	var w = Wrapper{d.Name, "string"}
	return json.Marshal(w)
}

var _ Schema = StructSchema{}
var _ Schema = DoubleSchema{}
var _ Schema = StringSchema{}

type AdvertiseWrapper struct {
	Type     string    `json:"type"`
	Channels []Channel `json:"channels"`
}

type Data interface {
	// MarshalJSON() ([]byte, error)
	Name() string
	Value() any
}

var _ Data = NumberData{}
var _ Data = StringData{}
var _ Data = StructData{}

type NumberData struct {
	Name_ string
	Val   float32
}

func (n NumberData) Name() string { return n.Name_ }

func (n NumberData) Value() any {
	return n.Val
}

type StringData struct {
	Name_ string
	Val   string
}

func (s StringData) Name() string {
	return s.Name_
}

// func (s StringData) MarshalJSON() ([]byte, error) {
// return json.Marshal(s.Val)
//
// }
func (s StringData) Value() any {
	return s.Val
}

type StructData struct {
	Name_ string `json:"name"`
	Val   []Data
}

func (s StructData) Name() string {
	return s.Name_
}

func (s StructData) MarshalJSON() ([]byte, error) {
	m := map[string]any{}
	for _, f := range s.Val {
		m[f.Name()] = f.Value()

	}
	return json.Marshal(map[string]any{s.Name(): m})
}
func (s StructData) Value() any {
	m := map[string]any{}
	for _, f := range s.Val {
		m[f.Name()] = f.Value()

	}
	return m
}

type DataHolder struct {
	Channel int  `json:"channel_id"`
	Data    Data `json:"data"`
}

func (d DataHolder) MarshalJSON() ([]byte, error) {
	type Wrapper struct {
		Type    string `json:"type"`
		Channel int    `json:"channel_id"`
		Data    any    `json:"data"`
	}
	return json.Marshal(Wrapper{"data", d.Channel, d.Data})
}

type Channel struct {
	ChannelId int    `json:"channel_id"`
	Schema    Schema `json:"schema"`
}

var addr = flag.String("addr", "localhost:8080", "http service address")

var upgrader = websocket.Upgrader{} // use default options

func echo(initialChannels []Channel, msgs chan Message, w http.ResponseWriter, r *http.Request) {
	c, err := upgrader.Upgrade(w, r, nil)
	if err != nil {
		log.Print("upgrade:", err)
		return
	}

	adv := AdvertiseWrapper{
		Type:     "advertisement",
		Channels: initialChannels,
	}
	initSchema, err := json.Marshal(adv)
	if err != nil {
		log.Println("Error marshalling initial info", err)
		return
	}
	c.WriteMessage(websocket.TextMessage, initSchema)
	type WSMessage struct {
		typ  int
		data []byte
		err  error
	}
	fromWS := make(chan WSMessage)
	go func() {
		for {
			mt, message, err := c.ReadMessage()
			if err != nil {
				log.Println("read err:", err)
				break
			}
			fromWS <- WSMessage{mt, message, err}

		}
	}()
	defer c.Close()

	shouldEnd := false
	for !shouldEnd {

		select {
		case wsmsg := <-fromWS:
			fmt.Println("From Websocket (UI->Board unhandled rn)", wsmsg)
		case brainMsg := <-msgs:
			fmt.Println("From Brain", brainMsg)
			if brainMsg.maybeChannels != nil {
				err = c.WriteJSON(brainMsg.maybeChannels)
				if err != nil {
					log.Println("Error writing json of channels")
					shouldEnd = true
				}
			} else if brainMsg.maybeData != nil {
				err = c.WriteJSON(DataHolder{
					Channel: brainMsg.ChannelId,
					Data:    brainMsg.maybeData,
				})
				if err != nil {
					log.Println("Error writing json of data")
					shouldEnd = true
				}
			}

		}
	}
}

func main() {
	c1 := StructSchema{
		Name: "motor",
		Fields: []Schema{
			DoubleSchema{"Voltage(V)"},
			DoubleSchema{"Current(A)"},
		},
	}
	c2 := DoubleSchema{
		Name: "value",
	}
	channels := []Channel{{0, c1}, {1, c2}}

	var fromBoard = make(chan Message)
	flag.Parse()
	log.SetFlags(0)
	http.HandleFunc("/ws", func(w http.ResponseWriter, r *http.Request) { echo(channels, fromBoard, w, r) })
	go func() {
		log.Fatal(http.ListenAndServe(*addr, nil))
	}()

	t := 0.0
	for {
		t += 0.01
		d1 := StructData{
			Name_: "motor",
			Val: []Data{
				NumberData{
					Name_: "Voltage(V)",
					Val:   2 + float32(math.Sin(t)),
				},
				NumberData{
					Name_: "Current(A)",
					Val:   0.2,
				},
			},
		}
		fromBoard <- Message{
			maybeChannels: nil,
			maybeData:     d1,
		}
		time.Sleep(10 * time.Millisecond)
	}
}

type Message struct {
	maybeChannels []Channel
	maybeData     Data
	ChannelId     int // only valid if maybeData != nil
}
