document.addEventListener('DOMContentLoaded', function () {
    const passwordIn = document.getElementById("KeyOneData");
    const btn = document.getElementById("togglePassword");


    btn.addEventListener('click', function () {
        const type = passwordIn.getAttribute('type') === 'password' ? 'text' : 'password';

        passwordIn.setAttribute('type', type);

        if (type === "password") {
            passwordIn.classList.add("text-green-500");
        } else {
            passwordIn.classList.add("text-gray-500");
            passwordIn.classList.remove("text-green-500");

        }
    });

});


document.addEventListener('DOMContentLoaded', function () {
    const encryptButton = document.getElementById('encryptButton');
    const results = document.getElementById('results');

    encryptButton.addEventListener('click', function () {
        // Toggle classes to show/hide the table with smooth transition
        results.classList.toggle('hidden-table');
        results.classList.toggle('visible-table');



    });
});



