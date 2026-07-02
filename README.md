# 藥物提醒 Pico W 專題

## 專題簡介

本專題為「智慧吃藥提醒與 LINE 雲端通知系統」，使用 Raspberry Pi Pico W 作為主控板，結合按鈕、蜂鳴器與 OLED 顯示器，建立一套適合長者或需定時服藥者使用的提醒系統。

當到達服藥時間時，系統會透過蜂鳴器與 OLED 顯示提醒使用者服藥。使用者完成服藥後，可按下按鈕進行確認。系統可透過 WiFi 傳送 LINE 通知，讓照顧者掌握服藥狀況，並將服藥資料記錄至 Google Sheets。

## 使用元件

- Raspberry Pi Pico W
- OLED 顯示器
- 蜂鳴器
- 按鈕開關
- 杜邦線
- 電阻

## 雲端服務

- LINE Messaging API
- Google Sheets

## 主要功能

1. 定時吃藥提醒
2. 蜂鳴器聲音提醒
3. OLED 顯示目前狀態
4. 按鈕確認服藥
5. LINE 雲端通知
6. Google Sheets 紀錄服藥資料

## 檔案說明

| 檔案名稱 | 說明 |
|---|---|
| README.md | 專題說明文件 |
| main.ino | Pico W 吃藥提醒系統主程式 |
| images/ | 專題照片與展示截圖 |
