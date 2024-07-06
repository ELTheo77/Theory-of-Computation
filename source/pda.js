function sendPostRequest(url, data, callback) {
    const xhr = new XMLHttpRequest();
    xhr.open('POST', url, true);
    xhr.setRequestHeader('Content-Type', 'text/plain');
    xhr.onload = function () {
        if (xhr.status === 200) {
            callback(null, JSON.parse(xhr.responseText));
        } else {
            callback(new Error('Request failed. Status: ' + xhr.status));
        }
    };
    xhr.onerror = function () {
        callback(new Error('Request failed. Network error.'));
    };
    console.log('Sending request to: ' + url);
    console.log('Request data: ' + data);
    xhr.send(data);
}

function handlePDAFormSubmit(event) {
    event.preventDefault();
    const pdaDefinition = document.getElementById('pda-definition').value;
    sendPostRequest('/pda', pdaDefinition, function (error, response) {
        if (error) {
            console.error('Error:', error);
        } else {
            const resultDiv = document.getElementById('pda-result');
            resultDiv.innerHTML = `<p>Converted CFG:</p><pre>${response.cfg}</pre>`;
        }
    });
}

document.getElementById('pda-form').addEventListener('submit', handlePDAFormSubmit);