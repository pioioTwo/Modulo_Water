// 
// This code turns an HTML table into scrollable list with multiple columns
//

var selected = null;
var visibilidade = false; // Variável que vai manipular o botão Exibir/ocultar do qifi Scan
//
// This function highlights a table row as the mouse hovers
// over it. It also adds code to mark a row as selected when 
// clicked on and toggle it when selected again
//
function tableHighlightRow() {
    if (document.getElementById && document.createTextNode) {
        var tables = document.getElementsByTagName('table');
        for (var i = 0; i < tables.length; i++) {
            if (tables[i].className === 'TableListJS') {
                var trs = tables[i].getElementsByTagName('tr');
                for (var j = 0; j < trs.length; j++) {
                    if (trs[j].parentNode.nodeName === 'TBODY') {
                        trs[j].onmouseover = function() {
                            // 'highlight' color is set in tablelist.css
                            if (this.className === '') {
                                this.className = 'highlight';
                            }
                            return false
                        }
                        trs[j].onmouseout = function() {
                                if (this.className === 'highlight') {
                                    this.className = '';
                                }
                                return false
                            }
                            // Função de seleção 
                        trs[j].onmousedown = function() {
                            //
                            // Toggle the selected state of this row
                            // 

                            // 'clicked' color is set in tablelist.css.
                            if (this.className !== 'clicked') {
                                // Clear previous selection
                                if (selected !== null) {
                                    selected.className = '';
                                }

                                // Mark this row as selected
                                this.className = 'clicked';
                                selected = this;

                                var tabela = document.getElementById("table_redes_encontradas");
                                var selecionado = tabela.getElementsByClassName("TableListJS");

                                var dados = "";

                                var selecionado = selected;
                                selecionado = selecionado.getElementsByTagName("td");
                                dados += "ID: " + selecionado[0].innerHTML + " - SIGNAL: " + selecionado[1].innerHTML + " - SSID: " + selecionado[2].innerHTML + " - LOK: " + selecionado[3].innerHTML + "\n";
                                td_array_selected(selecionado[0].innerHTML, selecionado[1].innerHTML, selecionado[2].innerHTML, selecionado[3].innerHTML);

                            } else {
                                this.className = '';
                                selected = null;
                                document.getElementById('input_ssid').value = "SSID"; // Passo o valor seleiciona 
                            }

                            return true
                        }
                    }
                }
            }
        }
    }
}


// Carrega as redes disponiveis na tabela



function addRow(col1Text, col1Width,
    col2Text, col2Width,
    col3Text, col3Width,
    col4Text, col4Width) {
    if (!document.getElementsByTagName) {
        return;
    }
    var x = document.getElementById("table_redes_encontradas").rows.length;
    tabBody = document.getElementsByTagName("tbody").item(0);
    //tabBody = document.getElementsByTagName("tbody").item(0);
    row = document.createElement("tr");
    row.id = x;
    cell1 = document.createElement("td");
    cell2 = document.createElement("td");
    cell3 = document.createElement("td");
    cell4 = document.createElement("td");
    cell1.width = col1Width + "px";
    cell2.width = col2Width + "px";
    cell3.width = col3Width + "px";
    cell4.width = col4Width + "px";
    textnode1 = document.createTextNode(col1Text);
    textnode2 = document.createTextNode(col2Text);
    textnode3 = document.createTextNode(col3Text);
    textnode4 = document.createTextNode(col4Text);
    cell1.appendChild(textnode1);
    cell2.appendChild(textnode2);
    cell3.appendChild(textnode3);
    cell4.appendChild(textnode4);
    row.appendChild(cell1);
    row.appendChild(cell2);
    row.appendChild(cell3);
    row.appendChild(cell4);
    tabBody.appendChild(row);
    tableHighlightRow();
}

window.onload = function() {

    // Teste
    var num_redes_disponiveis = 10;
    var id_redes_disponiveis = [num_redes_disponiveis];
    var ssid_redes_disponiveis = [num_redes_disponiveis];
    var signal_redes_disponiveis = [num_redes_disponiveis];
    var crip_redes_disponiveis = [num_redes_disponiveis];


    for (v = 0; v < num_redes_disponiveis; v++) {
        id_redes_disponiveis[v] = v;
        ssid_redes_disponiveis[v] = 'teste ' + v;
        signal_redes_disponiveis[v] = 's_' + v;
        crip_redes_disponiveis[v] = 'close';
        //console.log(ssid_redes_disponiveis[v]);
    }

    // teste

    for (r = 0; r < num_redes_disponiveis; r++) {
        addRow(
            id_redes_disponiveis[r], 30, // 100px width 
            signal_redes_disponiveis[r], 100,
            ssid_redes_disponiveis[r], 200,
            crip_redes_disponiveis[r], 72,
        );
    }

}

function toggleText(button_id) {
    var text = document.getElementById(button_id).firstChild;
    text.data = text.data == "Ocultar Redes" ? "Buscar Redes" : "Ocultar Redes";
}

function td_array_selected(id, signal, ssid, lock) {

    document.getElementById('input_ssid').value = ssid; // Passo o valor seleiciona 
    //alert("Pressionado");
}


function exibir() {
    document.getElementById("div_lista_redes_disponiveis").style.display = "block";
}

function ocultar() {
    document.getElementById("div_lista_redes_disponiveis").style.display = "none";
    document.getElementById("btnExiber").style.display = "none";
}

function ocultarExibir() { // Quando clicar no botão.

    if (visibilidade) { //Se a variável visibilidade for igual a true, então...
        document.getElementById("div_lista_redes_disponiveis").style.display = "none"; //Ocultamos a div
        visibilidade = false; //alteramos o valor da variável para falso.
    } else { //ou se a variável estiver com o valor false..
        document.getElementById("div_lista_redes_disponiveis").style.display = "block"; //Exibimos a div..
        visibilidade = true; //Alteramos o valor da variável para true.
    }
    toggleText('btnExibirOcultar');
}

window.addEventListener("load", function(event) {
    console.log("Todos os recursos terminaram o carregamento!");
});


function escreveMqttTopic() {

    document.getElementById('input_ssid').value = "SSID"; // Passo o valor seleiciona 
}

function ValidateIPaddress(inputText) {
    var ipformat = /^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$/;
    if (inputText.value.match(ipformat)) {
        document.form1.text1.focus();
        return true;
    } else {
        alert("You have entered an invalid IP address!");
        document.form1.text1.focus();
        document.this.focus();
        return false;
    }
}

function limparDataInvalida(campo) {

    console.log(campo.value);

    if (!VerificaData(campo.value)) {
        campo.value = '';

    }
    document.getElementById('topicMqtt').value = campo.value;

}

function VerificaData(digData) {


    return true;
}