let RADIUS = 40;
let CHEVRON = RADIUS/4;
let SELECTAREA = 10;
let FONTSIZE = 16;
let EPSILON = String.fromCharCode(949);
let SIGMA = ['a','b', 'c'];
let STATEFILL = "#fcfcfc";
let BLACK = "#000000";
let RED = "#ff0000";
const nodes = [];
var edges = [];
var sid = 0;
var tid = 0;
var highSid = -1;
var highTid = -1;
var startSid = -1;
var startTid = -1;

class Regex {

    constructor() {
        this.generate();
    }

    generate() {
        this.postfix = "";
        this.regex = this.#kleene(7, 0.5, 0.2, 0.1);
        this.nfa = this.#regexToNfa(this.postfix);
    }

    #regexToNfa(regex) {
        const nfa = [];
        const s = [];
        var start = 0;
        var end = 1;
        var count = 0;
        var c1 = 0;
        var c2 = 0;

        for (var i=0; i<regex.length; i++) {
            if (regex[i] == '*') {
                var top = s.pop();
                var r1 = top[0];
                var r2 = top[1];
                c1 = count++;
                c2 = count++;
                s.push([c1, c2]);
                nfa.push({});
                nfa.push({});
                for (var char of SIGMA) {
                    nfa[c1][char] = [];
                    nfa[c2][char] = [];
                }
                nfa[c1][EPSILON] = [];
                nfa[c2][EPSILON] = []
                nfa[r2][EPSILON].push(r1, c2);
                nfa[c1][EPSILON].push(r1, c2);
                if (start == r1) {
                    start = c1;
                }
                if (end == r2) {
                    end = c2;
                }
            } else if (regex[i] == '.') {
                var top1 = s.pop();
                var top2 = s.pop();
                var r11 = top1[0];
                var r12 = top1[1];
                var r21 = top2[0];
                var r22 = top2[1];
                s.push([r21, r12]);
                nfa[r22][EPSILON].push(r11);
                if (start == r11) {
                    start = r21;
                }
                if (end == r22) {
                    end = r12;
                }
            } else if (regex[i] == '+') {
                c1 = count++;
                c2 = count++;
                nfa.push({});
                nfa.push({});
                for (var char of SIGMA) {
                    nfa[c1][char] = [];
                    nfa[c2][char] = [];
                }
                nfa[c1][EPSILON] = [];
                nfa[c2][EPSILON] = []
                var top1 = s.pop();
                var top2 = s.pop();
                var r11 = top1[0];
                var r12 = top1[1];
                var r21 = top2[0];
                var r22 = top2[1];
                s.push([c1,c2]);
                nfa[c1][EPSILON].push(r21, r11);
                nfa[r12][EPSILON].push(c2);
                nfa[r22][EPSILON].push(c2);
                if (start == r11 || start == r21) {
                    start = c1;
                }
                if (end == r22 || end == r12) {
                    end = c2;
                }
            } else {
                c1 = count++;
                c2 = count++;
                nfa.push({});
                nfa.push({});
                for (var char of SIGMA) {
                    nfa[c1][char] = [];
                    nfa[c2][char] = [];
                }
                nfa[c1][EPSILON] = [];
                nfa[c2][EPSILON] = []
                s.push([c1,c2]);
                nfa[c1][regex[i]].push(c2);
            }
        }

        return {
            "table" : nfa,
            "start" : start,
            "end" : end
        }
    }

    #kleene(n, probOr, probKleene, probEmpty) {
        var expr = this.#expression(n, probOr, probKleene, probEmpty);
        if (Math.random() <= probKleene) {
            if (expr.length > 1) {
                expr = "(" + expr + ")*";
            } else {
                expr = expr + "*";
            }
            this.postfix += "*";
        }
        return expr;
    }

    #expression(n, probOr, probKleene, probEmpty) {
        if (n < 2) {
            var symbol = SIGMA[Math.floor(Math.random() * SIGMA.length)];
            this.postfix += symbol;
            return symbol;
        } else if (Math.random() <= probEmpty) {
            this.postfix += EPSILON;
            var after = this.#kleene(n-1, probOr, probKleene, probEmpty);
            this.postfix += "+";
            return "(" + EPSILON + " + " + after + ")";
        }

        var beforeSize = Math.floor(n/2);

        var before = this.#kleene(beforeSize, probOr, probKleene, probEmpty);
        var after = this.#kleene(n-beforeSize, probOr, probKleene, probEmpty);

        if (Math.random() <= probOr) {
            this.postfix += "+";
            return "(" + before + " + " + after + ")";
        }

        this.postfix += ".";
        return before + after;
    }

}

class Edge {

    constructor(id, fromNode, toNode) {
        this.id = id;
        this.fromNode = fromNode;
        this.toNode = toNode;
        this.label = "";

        this.x = null;
        this.y = null;
        this.radius = null;

        this.angle = null;

        this.curved = false;
    }

    draw(ctx) {

        ctx.strokeStyle = BLACK;
        ctx.fillStyle = BLACK;

        if (this.id == highTid) {
            ctx.strokeStyle = RED;
            ctx.fillStyle = RED;
        }

        ctx.beginPath();

        if (this.fromNode == this.toNode) {
            this.angle = 5*Math.PI/16;
            var dx = Math.cos(this.angle)*RADIUS;
            var dy = Math.sin(this.angle)*RADIUS;
            var xn = this.fromNode.x;
            var yn = this.fromNode.y;

            var x1 = xn-dx;
            var y1 = yn-dy;
            var x2 = xn+dx;
            var y2 = yn-dy;
            var x3 = xn;
            var y3 = yn-1.7*RADIUS;

            var circle = circleFromPoints(x1, y1, x2, y2, x3, y3);

            this.x = circle.x;
            this.y = circle.y;
            this.radius = circle.radius;

            var alpha = Math.atan2(y2-this.y, x2-this.x); 

            ctx.beginPath();
            ctx.arc(this.x, this.y, this.radius, Math.PI-alpha, alpha);
            ctx.stroke();

            ctx.beginPath();
            ctx.moveTo(x2, y2);
            ctx.lineTo(x2+CHEVRON*Math.cos(this.angle-Math.PI/10), y2-CHEVRON*Math.sin(this.angle-Math.PI/10));
            ctx.lineTo(x2-CHEVRON*Math.cos(this.angle+Math.PI/10), y2-CHEVRON*Math.sin(this.angle+Math.PI/10));
            ctx.closePath();
            ctx.stroke();
            ctx.fill();

            ctx.strokeStyle = BLACK;

            ctx.fillStyle = STATEFILL;
                
            var width = ctx.measureText(this.label).width;

            ctx.fillRect(x3-width/2, y3-4-FONTSIZE+2, width, FONTSIZE+2);

            ctx.fillStyle = BLACK;

            ctx.beginPath();
            ctx.fillText(this.label, x3, y3-4);
            ctx.stroke();

            ctx.fillStyle = STATEFILL
        } else if (this.curved) {
            var x1 = this.fromNode.x;
            var y1 = this.fromNode.y;

            var x2 = this.toNode.x;
            var y2 = this.toNode.y;

            var dx = x1-x2;
            var dy = y1-y2;
            
            this.angle = Math.atan2(dy, dx);

            var x3 = 0.5*(x1+x2) + 2*SELECTAREA*Math.cos(this.angle - Math.PI/2);
            var y3 = 0.5*(y1+y2) + 2*SELECTAREA*Math.sin(this.angle - Math.PI/2);

            var circle = circleFromPoints(x1, y1, x2, y2, x3, y3);

            var xc = circle.x;
            var yc = circle.y;

            var startAngle = Math.atan2(y2-yc, x2-xc);
            var endAngle = Math.atan2(y1-yc, x1-xc);

            ctx.beginPath();
            ctx.arc(xc, yc, circle.radius, startAngle, endAngle);
            ctx.stroke();

            var alpha = Math.acos(RADIUS/(2*circle.radius)) - startAngle + Math.PI;

            var xi = x2 + RADIUS*Math.cos(alpha);
            var yi = y2 - RADIUS*Math.sin(alpha);

            var beta = Math.atan2(yi-y2,xi-x2);
            
            ctx.beginPath();
            ctx.moveTo(xi, yi);
            ctx.lineTo(xi+CHEVRON*Math.cos(beta-Math.PI/5), yi+CHEVRON*Math.sin(beta-Math.PI/5));
            ctx.lineTo(xi+CHEVRON*Math.cos(beta+Math.PI/5), yi+CHEVRON*Math.sin(beta+Math.PI/5));
            ctx.closePath();
            ctx.stroke();
            ctx.fill();

            ctx.strokeStyle = BLACK;

            ctx.fillStyle = STATEFILL;
                
            var width = ctx.measureText(this.label).width;

            ctx.fillRect(x3-width/2, y3-FONTSIZE+2, width, FONTSIZE+2);

            ctx.fillStyle = BLACK;

            ctx.beginPath();
            ctx.fillText(this.label, x3, y3);
            ctx.stroke();

            ctx.fillStyle = STATEFILL;
        } else {
            if (this.id == startTid) {
                var toX = this.toNode.x-RADIUS;
                var toY = this.toNode.y;
                var fromX = toX-RADIUS;
                var fromY = toY;
                var dx = RADIUS;
                var dy = 0;
                this.angle = Math.atan2(dy, dx);
            } else {
                var toX = this.toNode.x;
                var toY = this.toNode.y;
                var fromX = this.fromNode.x;
                var fromY = this.fromNode.y;

                var dx = toX-fromX;
                var dy = toY-fromY;
                this.angle = Math.atan2(dy, dx);

                fromX += Math.cos(this.angle)*RADIUS;
                fromY += Math.sin(this.angle)*RADIUS;
                toX -= Math.cos(this.angle)*RADIUS;
                toY -= Math.sin(this.angle)*RADIUS;
            }

            ctx.beginPath();
            ctx.moveTo(fromX, fromY);
            ctx.lineTo(toX, toY);
            ctx.stroke();

            ctx.beginPath();
            ctx.moveTo(toX, toY);
            ctx.lineTo(toX-CHEVRON*Math.cos(this.angle - Math.PI/6), toY-CHEVRON*Math.sin(this.angle - Math.PI/6));
            ctx.lineTo(toX-CHEVRON*Math.cos(this.angle + Math.PI/6), toY-CHEVRON*Math.sin(this.angle + Math.PI/6));
            ctx.closePath();
            ctx.stroke();
            ctx.fill();

            ctx.strokeStyle = BLACK;
            ctx.fillStyle = STATEFILL;

            if (this.fromNode != null) {

                var width = ctx.measureText(this.label).width;

                var x = (this.fromNode.x + this.toNode.x) / 2;
                var y = (this.fromNode.y + this.toNode.y) / 2;

                ctx.fillRect(x-width/2, y-FONTSIZE+2, width, FONTSIZE+2);

                ctx.fillStyle = BLACK;

                ctx.beginPath();
                ctx.fillText(this.label, x, y);
                ctx.stroke();

                ctx.fillStyle = STATEFILL;
            }
        }
    }
}


class Node {

    constructor(id, x, y) {
        this.id = id;
        this.x = x;
        this.y = y;
        this.label = "";
        this.accept = false;
        this.dragging = false;
    }

    draw(ctx) {
        if (this.id == highSid) {
            ctx.strokeStyle = RED;
        }

        ctx.beginPath();
        ctx.arc(this.x, this.y, RADIUS, 0, 2*Math.PI);
        ctx.fill();
        ctx.stroke();

        if (this.accept) {
            ctx.beginPath();
            ctx.arc(this.x, this.y, RADIUS-8, 0, 2*Math.PI);
            ctx.fill();
            ctx.stroke();
        }

        ctx.strokeStyle = BLACK;

        ctx.fillStyle = BLACK;
        ctx.beginPath();
        ctx.fillText(this.label, this.x, this.y+5);
        ctx.stroke();

        ctx.fillStyle = STATEFILL;
    }
}

function isomorphic(user, regex) {
    const accept = user.accept.concat(regex.accept);
    const parent = {};
    const rank = {};
    const pairStack = [];
    const userStates = [];
    for (var k of Object.keys(user.dfa)) {
        userStates.push(parseInt(k));
    }

    for (var s of Object.keys(user.dfa)) {
        makeSet(parseInt(s), parent, rank);
    }
    for (var s of Object.keys(regex.dfa)) {
        makeSet(parseInt(s), parent, rank);
    }

    var equal = true;

    equal = unionCheck(user.start, regex.start, parent, rank, accept);

    pairStack.push([user.start, regex.start]);

    while (pairStack.length > 0 && equal) {
        pair = pairStack.pop();
        for (var c of SIGMA) {
            var r1 = 0;
            var r2 = 0;
            if (userStates.includes(pair[0])) {
                r1 = findSet(user.dfa[pair[0]][c], parent);
            } else {
                r1 = findSet(regex.dfa[pair[0]][c], parent);
            }
            if (userStates.includes(pair[1])) {
                r2 = findSet(user.dfa[pair[1]][c], parent);
            } else {
                r2 = findSet(regex.dfa[pair[1]][c], parent);
            }
            if (r1 != r2) {
                equal = unionCheck(r1, r2, parent, rank, accept);
                pairStack.push([r1, r2]);
            }
        }
    }

    return equal;
}

function makeSet(x, parent, rank) {
    parent[x] = x;
    rank[x] = 0;
}

function unionCheck(x, y, parent, rank, accept) {
    var a = findSet(x, parent);
    var b = findSet(y, parent);
    if (accept.includes(a)) {
        if (!accept.includes(b)) {
            return false;
        }
    } else {
        if (accept.includes(b)) {
            return false;
        }
    }
    link(a, b, parent, rank);
    return true;
}

function link(x, y, parent, rank) {
    if (rank[x] > rank[y]) {
        parent[y] = x;
    } else {
        parent[x] = y;
        if (rank[x] == rank[y]) {
            rank[y] += 1;
        }
    }
}

function findSet(x, parent) {
    if (x != parent[x]) {
        parent[x] = findSet(parent[x], parent);
    }
    return parent[x];
}

function subsetConstruct(nfa, start, final, dfaId) {
    const accept = [];

    var begin = dfaId;

    const dfa = {};
    const dfaIds = {};

    const nodeClosure = [];
    for (const [n, t] of Object.entries(nfa)) {
        nodeClosure[n] = [];
    }

    var firstState = eClose([start], nodeClosure, nfa);
    dfa[dfaId] = {};
    dfaIds[firstState] = dfaId++;
    for (var n of firstState) {
        if (final.includes(n)) {
            accept.push(dfaIds[firstState]);
            break;
        }
    }

    const nodeQueue = [firstState];
    
    while (nodeQueue.length > 0) {
        var currentState = nodeQueue.shift();
        for (var s of SIGMA) {
            var subset = nodeSubset(currentState, s, nodeClosure, nfa).sort();
            if (!(subset in dfaIds)) {
                dfa[dfaId] = {};
                dfaIds[subset] = dfaId++;
                for (var n of subset) {
                    if (final.includes(n)) {
                        accept.push(dfaIds[subset]);
                        break;
                    }
                }
                nodeQueue.push(subset);
            }
            dfa[dfaIds[currentState]][s] = dfaIds[subset];
        }
    }

    return {
        dfa : dfa,
        start : begin,
        accept : accept
    }
}

function nodeSubset(states, symbol, nodeClosure, nfa) {
    var subset = new Set();
    for (var s of states) {
        for (var t in nfa[s]) {
            if (t == symbol) {
                for (var n of nfa[s][t]) {
                    subset.add(n);
                }
            }
        }
    }
    var nodeIds  = [];
    for (var n of subset.values()) {
        nodeIds.push(n);
    }
    return eClose(nodeIds, nodeClosure, nfa);
}

function eClose(states, nodeClosure, nfa) {
    var closed = new Set();
    for (var n of states) {
        if (nodeClosure[n].length == 0) {
            var nClosed = new Set();
            var eStates = close(n, nodeClosure, nClosed, nfa);
            for (var q of eStates) {
                nodeClosure[n].push(q);
            }
        }
        for (var q of nodeClosure[n]) {
            closed.add(q);
        }
    }
    const values = [];
    for (var v of closed.values()) {
        values.push(v);
    }
    return values;
}

function close(k, nodeClosure, nClosed, nfa) {
    if (nodeClosure[k].length == 0) {
        nClosed.add(k);
        if (nfa[k].length != 0 && EPSILON in nfa[k]) {
            for (var q of nfa[k][EPSILON]) {
                if (!nClosed.has(q)) { 
                    for (var p of close(q, nodeClosure, nClosed, nfa)) { 
                        nClosed.add(p);
                    }
                }
            }
        }
    } else {
        for (var q of nodeClosure[k]) { 
            nClosed.add(q);
        }
    }
    return nClosed.values();
}

function getSymbols(label) {
    var s = new Set();
    const alphabet = [];
    for (var c of SIGMA) {
        alphabet.push(c);
    }
    alphabet.push(EPSILON);
    for (var char of label) {
        if (alphabet.includes(char)) {
            s.add(char);
        }
    }
    return s.values();
}

function transTable() {
    const nfa = {};
    const final = [];
    const alphabet = [];
    for (var c of SIGMA) {
        alphabet.push(c);
    }
    alphabet.push(EPSILON);
    for (var n of nodes) {
        nfa[n.id] = {};
        for (var s of alphabet) {
            nfa[n.id][s] = [];
        }
        if (n.accept) {
            final.push(n.id);
        }
    }
    for (var e of edges) {
        var symbols = getSymbols(e.label);
        for (var s of symbols) {
            nfa[e.fromNode.id][s].push(e.toNode.id);
        }
    }

    return {
        table : nfa,
        accept : final
    }
}

function circleFromPoints(x1, y1, x2, y2, x3, y3) {
    var a = x1*(y2-y3)-y1*(x2-x3)+x2*y3-x3*y2;
    var b = (x1**2+y1**2)*(y3-y2)+(x2**2+y2**2)*(y1-y3)+(x3**2+y3**2)*(y2-y1);
    var c = (x1**2+y1**2)*(x2-x3)+(x2**2+y2**2)*(x3-x1)+(x3**2+y3**2)*(x1-x2);

    var x = -b/(2*a);
    var y = -c/(2*a);

    return {
        'x' : x,
        'y' : y,
        'radius' : Math.hypot(x-x1, y-y1)
    }
}

function getFromId(id, arr) {
    for (var i=0; i<arr.length; i++) {
        if (arr[i].id == id) {
            return i;
        }
    }
}

function edgeUnderMouse(xm, ym) {
    for (var i=edges.length-1; i >=0; i--) {
        var edge = edges[i];
        if (edge.id != startTid) {
            if (edge.fromNode == edge.toNode) {
                var dx = edge.x-xm;
                var dy = edge.y-ym;
                if (dx*dx+dy*dy < (edge.radius+SELECTAREA)*(edge.radius+SELECTAREA)) {
                    return i;
                }
            } else {
                var xf = edge.fromNode.x;
                var yf = edge.fromNode.y;
                var xt = edge.toNode.x;
                var yt = edge.toNode.y;
                var dx = xt - xf;
                var dy = yt - yf;
                var len = Math.sqrt(dx*dx+dy*dy);
                if (edge.curved) { 
                    var ang = 1.5*Math.PI-edge.angle;
                    var cosShift = 2*SELECTAREA*Math.cos(ang);
                    var sinShift = 2*SELECTAREA*Math.sin(ang);
                    var perc = (dx*(xm-xf+cosShift)+dy*(ym-yf-sinShift))/(len*len);
                    var dist = (dx*(ym-yf-sinShift)-dy*(xm-xf+cosShift))/len;
                } else { 
                    var perc = (dx*(xm-xf)+dy*(ym-yf))/(len*len); 
                    var dist = (dx*(ym-yf)-dy*(xm-xf))/len; 
                }
                if (perc > 0 && perc < 1 && Math.abs(dist) < SELECTAREA) {
                    return i;
                }
            }
        }
    }
    return -1;
}

function nodeUnderMouse(x, y) {
    for (var i=nodes.length-1; i >= 0; i--) {
        var node = nodes[i];
        var dx = node.x-x;
        var dy = node.y-y;
        if (dx*dx+dy*dy < RADIUS*RADIUS) {
            return i;
        }
    }
    return -1;
}

function coordinates(event) {
    var dimensions = canvas.getBoundingClientRect();
    return {
        x: event.clientX-dimensions.left,
        y: event.clientY-dimensions.top
    }
}

function updateCanvas(mouseDown) {
    if (state && (state.dragging || mouseDown)) {
        ctx.clearRect(0, 0, canvas.width, canvas.height);

        for (var i=0; i<edges.length; i++) {
            if (edges[i].id != startTid) {
                edges[i].draw(ctx);
            }
        }

        for (var j=0; j<nodes.length; j++) {
            nodes[j].draw(ctx);
            if (nodes[j].id == startSid) {
                edges[getFromId(startTid, edges)].draw(ctx);
            }
        }
    }
}

function dfaTest(table) {
    for (const symbols of Object.values(table)) {
        for (const [symbol, ids] of Object.entries(symbols)) {
            if (symbol != EPSILON) {
                if (ids.length != 1) {
                    return false;
                }
            } else {
                if (ids.length != 0) {
                    return false;
                }
            }
        }
    }
    return true;
}

function comp() {
    if (startSid != -1) {
        var proceed = true;
        var userNFA = transTable();
        if (document.getElementById('dfa-toggle').checked) {
            proceed = dfaTest(userNFA.table);
        }
        if (proceed) {
            var regNFA = regularExpression.nfa;
            var user = subsetConstruct(userNFA.table, startSid, userNFA.accept, 0);
            var reg = subsetConstruct(regNFA.table, regNFA.start, [regNFA.end], Object.keys(user.dfa).length)
            var equal = isomorphic(user, reg);
            if (equal) {
                answer.innerHTML = "Correct";
                answer.style.color = "green";
            } else {
                answer.innerHTML = "Incorrect";
                answer.style.color = "red";
            }
        } else {
            answer.innerHTML = "Not a DFA";
            answer.style.color = "red";
        }
    } else {
        answer.innerHTML = "No Start State";
        answer.style.color = "red";
    }
}

function gen() {
    regularExpression.generate();
    regex.innerHTML = regularExpression.regex;
    answer.innerHTML = "Draw A Machine";
    answer.style.color = "black";
}

const canvas = document.getElementById('flat-canvas');
const regex = document.getElementById('regex');
const answer = document.getElementById('result');
const ctx = canvas.getContext('2d');
ctx.fillStyle = STATEFILL;
ctx.textAlign = "center";
ctx.font = FONTSIZE + "px Arial";
var fromX = 0;
var fromY = 0;

const regularExpression = new Regex();
regex.innerHTML = regularExpression.regex;
answer.innerHTML = "Draw A Machine";

var state = null;

var clickCount = localStorage.getItem('clickCount') || 0;

window.addEventListener("keydown",
    function(event){
		
		clickCount++; 
        localStorage.setItem('clickCount', clickCount);

        var addLabel = null;

        if (highSid != -1) { 
            index = getFromId(highSid, nodes);
            addLabel = nodes[index];
        } else if (highTid != -1) { 
            index = getFromId(highTid, edges);
            addLabel = edges[index];
        }

        if (addLabel != null && event.key.length == 1) {
            var length = addLabel.label.length;
            if (length > 0 && length < 7 && addLabel.label[length-1] == '\\' && event.key == 'e') {
                addLabel.label = addLabel.label.slice(0,-1) + EPSILON;
            } else if (length < 6) {
                addLabel.label += event.key;
            }
        } else if (event.key == "Backspace") {
            addLabel.label = addLabel.label.slice(0,-1);
        } else if (event.key == "Delete") { 
            if (highSid != -1) { 
                var new_edges = [];
                for (var i=0; i<edges.length; i++) {
                    if (edges[i].fromNode == nodes[index] || edges[i].toNode == nodes[index]) {
                        if (edges[i].id == startTid) {
                            startTid = -1;
                        }
                    } else {
                        new_edges.push(edges[i]);
                    }
                }
                edges = new_edges;
                if (nodes[index].id == startSid) {
                    startSid = -1;
                }
                nodes.splice(index,1);
                highSid = -1;
            } else if (highTid != -1) { 
                if (edges[index].id == startTid) {
                    startTid = -1;
                }
                for (var i=0; i<edges.length; i++) {
                    if (edges[i].fromNode == edges[index].toNode && edges[i].toNode == edges[index].fromNode) {
                        edges[i].curved = false;
                        break;
                    }
                }
                edges.splice(index,1);
                highTid = -1;
            }
        } else if (event.key == "Escape") {
            highSid = -1;
            highTid = -1;
        }

        updateCanvas(true);
    }
);

canvas.addEventListener("dblclick",
    function(event) {

        var coords = coordinates(event);
        var x = coords.x;
        var y = coords.y;
        var stateIndex = nodeUnderMouse(x, y);
        var edgeIndex = edgeUnderMouse(x, y);

        if (stateIndex != -1) { 
            nodes[stateIndex].accept = !nodes[stateIndex].accept;
        } else if (edgeIndex == -1) { 
            if (event.shiftKey) { 
                if (highSid != -1) { 
                    var n = new Node(sid, x, y);
                    state = n;
                    nodes.push(n);
                    var e = new Edge(tid, nodes[getFromId(highSid, nodes)], n);
                    sid++;
                    edges.push(e);
                    tid++;
                }
            } else if (event.ctrlKey) { 
                var n = new Node(sid, x, y);
                state = n;
                nodes.push(n);
                highSid = sid;
                highTid = -1;
                startSid = sid;
                sid++;
                if (startTid == -1) {
                    var e = new Edge(tid, null, n);
                    edges.push(e);
                    startTid = tid;
                    tid++;
                } else {
                    for (var i=0; i<edges.length; i++) {
                        if (edges[i].id == startTid) {
                            edges[i].toNode = nodes[getFromId(highSid, nodes)];
                            break;
                        }
                    }
                }
            } else { 
                var n = new Node(sid, x, y);
                state = n;
                nodes.push(n);
                highSid = sid;
                highTid = -1;
                sid++;
            }
        }
        updateCanvas(true);
    }
);

canvas.addEventListener("mousedown",
    function(event) {

        var coords = coordinates(event);
        var x = coords.x;
        var y = coords.y;
        var stateIndex = nodeUnderMouse(x, y);
        var edgeIndex = edgeUnderMouse(x, y);

        if (stateIndex != -1) { 
            if (event.shiftKey) { 
                if (highSid != -1) { 
                    var from = nodes[getFromId(highSid, nodes)];
                    var create = true;
                    var curve = false;
                    for (var i=0; i<edges.length; i++) {
                        if (create && edges[i].fromNode == from && edges[i].toNode == nodes[stateIndex]) {
                            create = false;
                        }
                        if (!curve && edges[i].fromNode == nodes[stateIndex] && edges[i].toNode == from) {
                            curve = true;
                            edges[i].curved = true;
                        }
                    }
                    if (create) {
                        var e = new Edge(tid, nodes[getFromId(highSid, nodes)], nodes[stateIndex]);
                        edges.push(e);
                        tid++;
                        e.curved = curve;
                    }
                }
            } else if (event.ctrlKey) { 
                state = nodes[stateIndex];
                highSid = state.id;
                highTid = -1;
                startSid = highSid; 

                if (startTid == -1) {
                    var e = new Edge(tid, null, state);
                    edges.push(e);
                    startTid = tid;
                    tid++;
                } else {
                    for (var i=0; i<edges.length; i++) {
                        if (edges[i].id == startTid) {
                            edges[i].toNode = nodes[getFromId(highSid, nodes)];
                            break;
                        }
                    }
                }
            } else {
                state = nodes[stateIndex];
                state.dragging = true;
                highSid = state.id;
                highTid = -1;
                canvas.style.cursor = "move";
            }
        } else if (edgeIndex != -1) {
            var edge = edges[edgeIndex];
            highTid = edge.id;
            highSid = -1;
        } 
        updateCanvas(true);
    }
);

canvas.addEventListener("mousemove",
    function(event) {
        var coords = coordinates(event); 
        var x = coords.x;
        var y = coords.y;
        var stateId = nodeUnderMouse(x,y);

        if (stateId != -1) {
            canvas.style.cursor = "move";
        } else {
            canvas.style.cursor = "auto";
        }

        var dx = x-fromX;
        var dy = y-fromY;
        fromX = x;
        fromY = y;

        if (state && state.dragging) { 
            state.x += dx;
            state.y += dy;
            updateCanvas(false);
        }
    }
);

canvas.addEventListener("mouseup",
    function(){
        if (state && state.dragging) {
            state.dragging = false;
            canvas.style.cursor = "auto";
        }
    }
);
