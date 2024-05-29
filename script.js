let port;
let receivedData = "";

async function sendData() {

    const inputData = document.getElementById("inputData").ariaValueMax;


    try {

        if (port && port.writable) {
            const writer = port.writable.getWriter();
            const formattedData = formatData(inputData);

            const dataArray = new TextEncoder().encode(formattedData);

            await writer.write(dataArray);

            console.log("Data Sent :", formattedData);
            writer.releaseLock();
        } else {
            console.error("Port is not initialized or not writable.");
        }
    } catch (error) {
        console.error("Error in sending data.", error);
    }

}

function formatData(data) {
    return data.trim();
}


async function connect() {

    try {

        port = await navigator.serial.requestPort({ filters: [{ usbVendorId: 0x0483 }] });
        console.log("Port :", port);

        await port.open({ baudDate: 115200, dataBits: 8, stopBits: 1, parity: "none" });
        console.log("Port opened :",port.getInfo());
        document.getElementById("status").innerText = "Connected.";
        document.getElementById("status").classList.add("text-green-500");
        document.getElementById("status").classList.remove("text-red-500");
        

        // Start reading the data from the port after connecting..
        
        readDataFromPort();

    } catch (error){
        console.error("Error in connecting port :",error);
        document.getElementById("status").innerText = "Disconnected!";
        document.getElementById("status").classList.add("text-red-500");
        document.getElementById("status").classList.remove("text-green-500");

    } 
}


function handleReceivedData(data){
    // Split the received data into lines.

    var lines = data.split('\n');

    // Assuming the first line contains the decrypted message and the second line contains the success message


    var decryptedMessage = lines[0];

    var successMessage = lines[1];

    var receivedData = document.getElementById("receivedData");

    receivedData.value = decryptedMessage;
}

async function readDataFromPort(){
    try {

        if(port && port.readable){
            const reader = port.readable.getReader();

            while(true){
                const {value,done} = await reader.read();
                if(done){
                    console.log("Data reading is complete...");
                    reader.releaseLock();
                    break;
                }

                handleReceivedData(new TextDecoder().decode(value));
            }
        } else {
            console.error("Port is not initialized or not readable..!");
        }
    } catch (error){
        console.error("Error in reading the data.");
    }
}


async function disconnect(){
    if(port && port.readable){
        await port.close();

        document.getElementById("status").innerText = "Disconnected!";
        document.getElementById("status").classList.add("text-red-500");
        document.getElementById("status").classList.remove("text-green-500");
    }
}

