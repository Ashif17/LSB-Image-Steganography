# 🧠 Steganography-Based File Encoder and Decoder in C

This project implements a **Steganography-based File Hiding System in C**, which enables users to securely **embed (encode)** and **extract (decode)** secret data within **BMP image files**. The technique uses **Least Significant Bit (LSB)** modification — a common method in digital steganography — to conceal information without noticeably altering the image.

The system works in two modes: **Encoding** and **Decoding**. In encoding mode, a secret file (such as `.txt`, `.c`, or `.sh`) is hidden inside a BMP image, generating a new **stego image**. In decoding mode, the system scans BMP files to detect and extract any hidden data that was previously embedded.

---

## 🔹 Core Functionalities

### 🔸 Encoding Mode (-e)
- Takes a BMP image and a secret file as input.
- Embeds the secret file’s data into the image using LSB manipulation.
- Produces a new encoded image (`encodedimage.bmp` by default).
- Encodes metadata like file extension, file size, and a unique magic string (`#*`) to validate hidden content.

### 🔸 Decoding Mode (-d)
- Accepts one or more BMP files as input.
- Detects the magic string to verify hidden data.
- Extracts and reconstructs the secret file with its original extension (`decoded_data.txt`, `.c`, or `.sh`).

### 🔸 File Validation
- Ensures valid input formats (`.bmp` for images and `.txt/.c/.sh` for secrets).
- Checks that the source image has enough capacity to store the secret file data.

### 🔸 Error Handling & User Guidance
- Displays clear usage instructions and validation messages.
- Provides detailed logs during encoding and decoding operations.

---

## 🔹 Technical Highlights

- Implements **bitwise operations** for encoding and decoding at the binary level.
- Demonstrates **modular programming** through separate source and header files (`encode.c`, `decode.c`, `types.h`, `common.h`).
- Employs **file handling** functions (`fopen`, `fread`, `fwrite`, `fseek`) to process both binary and text data.
- Validates inputs dynamically with user-friendly console feedback.

---

## 🔹 How to Use

### 🧩 Encoding Example
```bash
./a.out -e beautiful.bmp secret.txt encodedimage.bmp
```

### 🧩 Decoding Example
```bash
./a.out -d encodedimage.bmp
```

---

## 🔹 Summary

This project effectively demonstrates how **data hiding and retrieval** can be achieved through image steganography in C. It’s a great example of combining **theory and practice** in computer security, showcasing how binary manipulation and structured programming can be used for **secure communication and data concealment**.
