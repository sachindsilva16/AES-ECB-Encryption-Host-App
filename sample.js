function decryptDataECB(cipherText, key) {
    // Convert key to WordArray (Neccessary for CryptoJS)
    const keyWordArray = CryptoJS.enc.Utf8.parse(key);

    // Decrypt ciphertext
    const decrypted = CryptoJS.AES.decrypt(cipherText, keyWordArray, {
        mode: CryptoJS.mode.ECB,
        padding: CryptoJS.pad.NoPadding
    });

    // Convert decrypted WordArray to plaintext
    const decryptedText = CryptoJS.enc.Utf8.stringify(decrypted);

    return decryptedText;
}