let port;
let receivedData = "";

function formatData(data) {
    return data.trim();
}

async function connect() {
    try {
        port = await navigator.serial.requestPort({ filters: [{ usbVendorId: 0x0483 }] });
        console.log("Port:", port);

        await port.open({ baudRate: 115200, dataBits: 8, stopBits: 1, parity: "none" });
        console.log("Port opened:", port.getInfo());
        document.getElementById("status").innerText = "Connected.";
        document.getElementById("status").classList.add("text-green-500");
        document.getElementById("status").classList.remove("text-red-500");
        document.getElementById("inputData").removeAttribute("disabled");
        document.getElementById("encryptButton").removeAttribute("disabled");
    } catch (error) {
        console.error("Error in connecting port:", error);
        document.getElementById("status").innerText = "Disconnected!";
        document.getElementById("status").classList.add("text-red-500");
        document.getElementById("status").classList.remove("text-green-500");
    }
}

function handleReceivedData(data, userInputData) {
    var receivedDataTextarea = document.getElementById("receivedData");
    receivedDataTextarea.value += data;
    receivedDataTextarea.value += "\nUser Input Data: " + userInputData;
}

async function readDataFromPort(userInputData) {
    try {
        if (port && port.readable) {
            const reader = port.readable.getReader();
            while (true) {
                const { value, done } = await reader.read();
                if (done) {
                    console.log("Data reading is complete...");
                    reader.releaseLock();
                    break;
                }
                if (value) {
                    handleReceivedData(new TextDecoder().decode(value), userInputData);
                } else {
                    console.warn("Received empty data from the port.");
                }
            }
        } else {
            console.error("Port is not initialized or not readable.");
        }
    } catch (error) {
        console.error("Error in reading the data:", error);
    }
}

async function sendData() {
    try {
        const userInputData = document.getElementById("inputData").value;
        const formattedData = formatData(userInputData);
        
        if (port && port.writable) {
            const writer = port.writable.getWriter();
            const dataArray = new TextEncoder().encode(formattedData);
            await writer.write(dataArray);
            console.log("Data Sent:", formattedData);
            writer.releaseLock();
        } else {
            console.error("Port is not initialized or not writable.");
        }
        
        // After sending data, start reading data from the port
        readDataFromPort(userInputData);
    } catch (error) {
        console.error("Error in sending data:", error);
    }
}
