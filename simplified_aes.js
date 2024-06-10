let port;
let receivedData = "";

// function formatData(data) {
//     if (data === undefined || data === null) {
//         return '';
//     }
//    return data.trim();
// }

async function connect() {
    try {
        port = await navigator.serial.requestPort({ filters: [{ usbVendorId: 0x0483 }] });
        console.log("Port:", port);
        await port.open({ baudRate: 115200, dataBits: 8, stopBits: 1, parity: "none" });
        console.log("Port opened:", port.getInfo());
        document.getElementById("status").innerText = "Connected";
        document.getElementById("status").classList.add("text-green-500");
        document.getElementById("inputText").removeAttribute("disabled");
        document.getElementById("encryptData").removeAttribute("disabled");
    } catch (error) {
        console.error("Error in connecting port:", error);
        document.getElementById("status").innerText = "Disconnected";
        document.getElementById("status").classList.add("text-red-500");
        document.getElementById("status").classList.remove("text-green-500");
    }
}

function handleReceivedData(data) {
    const encryptedDataTextarea = document.getElementById("encryptedData");
    encryptedDataTextarea.value += data; // Append data to the existing content with a newline
}

async function readDataFromPort() {
    try {
        if (port && port.readable) {
            const reader = port.readable.getReader();
            while (true) {
                const { value, done } = await reader.read();
                if (done) {
                    console.log("Data reading is complete");
                    reader.releaseLock();
                    break;
                }
                if (value) {
                    handleReceivedData(new TextDecoder().decode(value));
                } else {
                    console.warn("Received empty data from the port");
                }
            }
        } else {
            console.error("Port is not initialized or not readable");
        }
    } catch (error) {
        console.error("Error in reading the data:", error);
    }
}

function encryptAES_ECB(text, key) {
    // Convert text and key to WordArrays (necessary for CryptoJS)
    const textWordArray = CryptoJS.enc.Utf8.parse(text);
    const keyWordArray = CryptoJS.enc.Utf8.parse(key);

    // Encrypt using AES ECB mode
    const encrypted = CryptoJS.AES.encrypt(textWordArray, keyWordArray, {
      mode: CryptoJS.mode.ECB,
      padding: CryptoJS.pad.Pkcs7,
    });

    // Return the encrypted data as a base64-encoded string
    return encrypted.toString();
  }

  function encrypt(plaintext,key) {

    const ciphertext = encryptAES_ECB(plaintext, key);
    return ciphertext;
  }



  async function sendData(event) {
    event.preventDefault();
    try {
        const userInputData = document.getElementById("inputText").value;
        const key = document.getElementById("key").value;
        const encryptedData = encrypt(userInputData, key);

        if (port && port.writable) {
            const writer = port.writable.getWriter();
            const dataArray = new TextEncoder().encode(encryptedData);
            await writer.write(dataArray);
            console.log("Data sent: ", encryptedData);

            // Clear previous output from the screen
            const encryptedDataTextarea = document.getElementById("encryptedData");
            encryptedDataTextarea.value = ''; // Clear the content of the textarea

            // Update the encryptedData textarea with the new encrypted data
            encryptedDataTextarea.value += encryptedData + '\n'; // Append the encrypted data to the textarea with a newline
            encryptedDataTextarea.scrollTop = encryptedDataTextarea.scrollHeight; // Scroll to the bottom of the textarea
            writer.releaseLock();
        } else {
            console.error("Port is not initialized or not writable");
        }
    } catch (error) {
        console.error("Error in sending data:", error);
    }
}



async function readDataFromPort() {
    try {
        if (port && port.readable) {
            const reader = port.readable.getReader();
            while (true) {
                const { value, done } = await reader.read();
                if (done) {
                    console.log("Data reading is complete");
                    reader.releaseLock();
                    break;
                }
                if (value) {
                    handleReceivedData(new TextDecoder().decode(value));
                } else {
                    console.warn("Received empty data from the port");
                }
            }
        } else {
            console.error("Port is not initialized or not readable");
        }
    } catch (error) {
        console.error("Error in reading the data:", error);
    }
}

async function sendDataAndReadData(event) {
    await sendData(event);
    await readDataFromPort();
}

function clearEncryptedData() {
    document.getElementById('encryptedData').value = "";
}

//  function formatData(userInputData) {
// //     // Encode the input data using Base64
// const encodedData = btoa(userInputData);
// return encodedData;
// //     let encryptedData = encryptData(userInputData);
    
// //     return encryptedData;
//  }

async function disconnect() {
    if (port && port.readable) {
        await port.close();
        document.getElementById("status").innerText = "Disconnected!";
        document.getElementById("status").classList.add("text-red-500");
        document.getElementById("status").classList.remove("text-green-500");
        document.querySelector('button[type="button"][onclick="sendData()"]').setAttribute("disabled", true);
    }
}