# R36s Smart System - ระบบอัจฉริยะสำหรับ R36s และโทรศัพท์

![R36s Smart System](https://img.shields.io/badge/version-2.0.0-brightgreen)
![License](https://img.shields.io/badge/license-MIT-blue)
![Platform](https://img.shields.io/badge/platform-Android%20Phone%20%7C%20R36s%20Device-orange)

> **ระบบควบคุมอัจฉริยะสำหรับอุปกรณ์ R36s พกพา พร้อมแอปพลิเคชันโทรศัพท์**

---

## 🎯 ภาพรวม

**R36s Smart System** เป็นแพลตฟอร์มควบคุมที่ทรงพลังสำหรับอุปกรณ์ R36s แบบพกพา พร้อมแอปพลิเคชันช่วยเหลือบนโทรศัพท์ Android ให้คุณจัดการอุปกรณ์ได้จากที่ไหนก็ได้

---

## ✨ คุณสมบัติหลัก

- 🎮 **ระบบควบคุมแบบเรียลไทม์** - ควบคุม R36s จากโทรศัพท์
- 📱 **แอปพลิเคชันโทรศัพท์ Android** - อินเตอร์เฟสที่สวยงาม
- ⚙️ **ตั้งค่าอุปกรณ์** - การกำหนดค่าแบบสมบูรณ์ของ R36s
- 🔄 **ซิงค์ข้อมูล** - ซิงค์อัตโนมัติระหว่างอุปกรณ์
- 📊 **การวิเคราะห์ประสิทธิภาพ** - ติดตามสถานะอุปกรณ์
- 💾 **จัดเก็บข้อมูลในเครื่อง** - ไม่มีการเชื่อมต่อคลาวด์ที่จำเป็น
- 🚀 **เร็วและเบา** - การใช้ทรัพยากรต่ำ

---

## 📥 การดาวน์โหลดและติดตั้ง

### 📱 Android Phone App
```bash
https://github.com/smi9la/androidos-rk/releases/download/v2.0.0/r36s-controller-2.0.0.apk
```

### 🎮 R36s Device Firmware
```bash
https://github.com/smi9la/androidos-rk/releases/download/v2.0.0/r36s-firmware-2.0.0.bin
```

---

## 🚀 เริ่มต้นอย่างรวดเร็ว

### ติดตั้งแอปโทรศัพท์
1. ดาวน์โหลด `r36s-controller-2.0.0.apk`
2. โอนไฟล์ไปยังโทรศัพท์
3. เปิดจากตัวจัดการไฟล์
4. อนุญาตการติดตั้ง

### ติดตั้ง R36s Firmware
1. ดาวน์โหลด `r36s-firmware-2.0.0.bin`
2. เชื่อมต่อ R36s กับคอมพิวเตอร์
3. ใช้เครื่องมือเฟิร์มแวร์เพื่อปรับปรุง
4. รีบูต R36s

---

## 📋 ข้อกำหนดของระบบ

| ส่วนประกอบ | ขั้นต่ำ | แนะนำ |
|---------|----------|----------|
| Android Phone | 6.0 API 23 | 10+ |
| R36s Device | V1.0 | V2.0+ |
| Storage | 100 MB | 500 MB |

---

## 🔧 สถาปัตยกรรมระบบ

```
┌─────────────────────┐
│  Android Phone      │
│  (Controller App)   │
└──────────┬──────────┘
           │ Bluetooth/WiFi
           │
┌──────────▼──────────┐
│   R36s Device       │
│  (Main System)      │
└─────────────────────┘
```

---

## 📚 เอกสารประกอบ

- [แนวทางการติดตั้ง](INSTALLATION.md)
- [คู่มือผู้ใช้](USER_GUIDE.md)
- [API Reference](API.md)

---

## 💻 ความต้องการในการพัฒนา

- Node.js 14+
- Java 8+
- Python 3.7+
- Android SDK
- Git

---

## 🔧 การสร้างจากซอร์สโค้ด

```bash
# ติดตั้งการพึ่งพา
pip3 install -r requirements.txt
npm install

# สร้างทั้งหมด
python3 BUILD_ALL.py

# หรือสร้างแพลตฟอร์มแยกต่างหาก
cd phone && ./gradlew build  # Android
cd ../r36s && make build    # R36s Device
```

---

## 📊 สถิติ

- **เวอร์ชัน**: 1 (R36s Controller)
- **ฟีเจอร์**: 12+
- **บรรทัดโค้ด**: 5000+
- **ไฟล์**: 20+

---

## 🐛 รายงานปัญหา

พบปัญหา? [บอกเราที่นี่](https://github.com/smi9la/androidos-rk/issues)

---

## 💡 คำขอใหม่

มีไอเดีย? [แบ่งปันที่นี่](https://github.com/smi9la/androidos-rk/discussions)

---

## 📄 ลิขสิทธิ์

MIT License - สำหรับทุกคน

---

## 👨‍💻 นักพัฒนา

**smi9la** - นักเขียนโปรแกรมและนักประดิษฐ์

---

## 🙏 ขอบคุณ

ขอบคุณที่ใช้ **R36s Smart System**! 🎮✨

---

**เวอร์ชัน**: v2.0.0
**วันที่เปิดตัว**: 2026-06-05
**สถานะ**: ✅ พร้อมใช้งาน