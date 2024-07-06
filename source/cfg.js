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

function handleCFGFormSubmit(event) {
    event.preventDefault();
    const cfgDefinition = document.getElementById('cfg-definition').value;
    sendPostRequest('/cfg', cfgDefinition, function (error, response) {
        if (error) {
            console.error('Error:', error);
        } else {
            const resultDiv = document.getElementById('cfg-result');
            resultDiv.innerHTML = `<p>Is Valid CFG: ${response.is_valid_cfg}</p>`;
        }
    });
}

document.getElementById('cfg-form').addEventListener('submit', handleCFGFormSubmit);