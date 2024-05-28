// VALIDATE INPUT FIELDS OF ENCRYPTION SECTION

function validateEncryptionInputs() {

    var plainTextArea = document.getElementById("plainTextArea");
    var secretKeyOne = document.getElementById("secretKeyOne");
    var isValid = true;


    // CHECK IF PLAINTEXT AREA IS EMPTY OR NOT

    if (plainTextArea.value.trim() === "") {
        plainTextArea.style.borderColor = "red";
        isValid = false;
    } else {
        plainTextArea.style.borderColor = "";
        // RESET THE BORDER COLOR
    }




    // CHECK IF THE SECRET KEY INPUT IS EMPTY OR NOT

    if (secretKeyOne.value.trim() === "") {
        secretKeyOne.style.borderColor = "red";
        isValid = false;
    } else {
        secretKeyOne.style.borderColor = "";
    }

    return isValid;
}


// VALIDATE THE INPUT FIELDS OF DECRYPTION FIELDS


function validateDecryptionInputs() {

    var cipherTextArea = document.getElementById("cipherTextArea");
    var secretKeyTwo = document.getElementById("secretKeyTwo");

    var isValid = true;


    // CHECK IF CIPHER TEXT AREA IS EMPTY OR NOT

    if (cipherTextArea.value.trim() === "") {
        cipherTextArea.style.borderColor = "red";
        isValid = false;
    } else {
        cipherTextArea.style.borderColor = "";
    }


    // CHECK IF THE SECRET KEY INPUT IS EMPTY OR NOT 

    if (secretKeyTwo.value.trim() === "") {
        secretKeyTwo.style.borderColor = "red";
        isValid = false;
    } else {
        secretKeyTwo.style.borderColor = "";
    }

    return isValid;

}


// EVENT LISTENER FOR ENCRYPT BUTTON CLICK...

document.getElementById("encryptButton").addEventListener("click",function(){

    if(validateEncryptionInputs()){

        alert("ENCRYPTION SUCCCESS!!");
    }
});


document.getElementById("decryptButton").addEventListener("click",function(){
    if(validateDecryptionInputs()){
        alert("DECRYPTION SUCCCESS!!");

    }
});







// document.getElementById("encryptButton").addEventListener("click",function(){
//     if(validateEncryptionInputs()){
//         encryptThePlainText();
//     };
// });



// function validateEncryptionInputs(){

//     var plainText = document.getElementById("plainTextArea").value;
//     var secretKeyOne = document.getElementById("secretKeyOne").value;

//     if(plainText.trim() === "" || secretKeyOne.trim() === ""){


//         document.getElementById("plainTextArea").style.borderColor = "red";
//         document.getElementById("plainTextArea").style.color="red";

//         alert("Please enter the plaintext and your secret key!!");
//         return false;
//     }
//     return true;
// }
