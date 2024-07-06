function sendPostRequest(url, data, callback) {
    const xhr = new XMLHttpRequest();
    xhr.open('POST', url, true);
    xhr.setRequestHeader('Content-Type', 'application/json');
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
    console.log('Request data: ' + JSON.stringify(data));
    xhr.send(JSON.stringify(data));
}


function handleDFAFormSubmit(event) {
    event.preventDefault();
    const dfaDefinition = document.getElementById('dfa-definition').value;
    const inputString = document.getElementById('input-string').value;
    const data = {
        dfaDefinition: dfaDefinition,
        inputString: inputString
    };
    sendPostRequest('/dfa', data, function (error, response) {
        if (error) {
            console.error('Error:', error);
        } else {
            console.log('Response:', response);
            const resultDiv = document.getElementById('dfa-result');
            resultDiv.innerHTML = `
                <p>Is Valid DFA: ${response.is_valid_dfa}</p>
                <p>Accepts Input String: ${response.accepts_input}</p>
            `;
        }
    });
}

document.getElementById('dfa-form').addEventListener('submit', handleDFAFormSubmit);