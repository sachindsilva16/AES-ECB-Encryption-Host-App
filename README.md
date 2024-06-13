# AES Encryption & Decryption with ECB Technique

## Description
This project demonstrates AES encryption and decryption using the Electronic Codebook (ECB) technique. It provides a user interface for encrypting and decrypting data using AES algorithm with ECB mode. The project includes both frontend (HTML, CSS, JavaScript) and backend (JavaScript) components.

## Features
- Connect to STM32 COM3 Port: Allows connecting to a specific COM port (e.g., STM32 board) for data transmission.
- AES Encryption & Decryption: Enables users to encrypt and decrypt data using AES with ECB mode.
- Real-time Data Transmission: Sends encrypted data to the connected device and receives decrypted data back for display.
- Visual Feedback: Provides visual feedback on connection status, encryption process, and result status.

## Technologies Used
- Frontend:
  - HTML5
  - CSS3 (Tailwind CSS for styling)
  - JavaScript (CryptoJS library for AES encryption/decryption)
- Backend:
  - JavaScript (for handling serial port communication and encryption/decryption logic)

## Installation
1. Clone the repository to your local machine:
``` git clone https://github.com/sachindsilva16/AES---ECB---User-Interface.git ```

2. Open the project directory:

3. Open `index.html` in your web browser to view the application.

## Usage
1. Connect to STM32 COM3 Port by clicking the "Connect" button.
2. Enter the data you want to encrypt in the input field.
3. Enter the encryption key in the provided field.
4. Click the "Send data" button to encrypt and send the data to the connected device.
5. Wait for the encrypted data to be received from the device and decrypted by the application.
6. View the encrypted and decrypted data, along with the encryption/decryption key and result status in the table.

## Development
To contribute to this project, follow these steps:
1. Fork the repository on GitHub.
2. Clone your forked repository to your local machine.
3. Create a new branch for your feature or bug fix.
4. Make changes and commit them to your branch.
5. Push your changes to your forked repository.
6. Create a pull request to merge your changes into the main repository.

## Credits
- **Developer**: [サチン・ドゥシルバ](https://github.com/sachindsilva16)
- **Icons**: Font Awesome (https://fontawesome.com/)
- **Fonts**: Inter (https://fonts.google.com/specimen/Inter)

## License
This project is licensed under the [MIT License](LICENSE).


## Preview of Host Application


[](https://github.com/sachindsilva16/AES---ECB---User-Interface/blob/main/Host%20App%20-%20Disconnected.png)

[](https://github.com/sachindsilva16/AES---ECB---User-Interface/blob/main/Host%20App%20-%20Connected.png)


[](https://github.com/sachindsilva16/AES---ECB---User-Interface/blob/main/Host%20application%20-%20Final%20output.png)