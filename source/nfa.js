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

function handleNFAFormSubmit(event) {
    event.preventDefault();
    const nfaDefinition = document.getElementById('nfa-definition').value;
    sendPostRequest('/nfa', nfaDefinition, function (error, response) {
        if (error) {
            console.error('Error:', error);
        } else {
            const resultDiv = document.getElementById('nfa-result');
            resultDiv.innerHTML = `<p>Converted DFA:</p><pre>${response.dfa}</pre>`;
        }
    });
}

document.getElementById('nfa-form').addEventListener('submit', handleNFAFormSubmit);