

function Login_Page_Loaded(){
    my_super_sekkrit_number = 0;

    Login_Form = window.document.getElementById("LoginForm");
    UserStuff = window.document.getElementById("UserStuff");
    Transfer_Form = window.document.getElementById("TransferForm");
    Menu = window.document.getElementById("Menu");
    TransactionHistory = window.document.getElementById("TransactionHistory");
    OtherUsers = window.document.getElementById("OtherUsers");
    ConfirmTransfer = window.document.getElementById("ConfirmTransfer");

    HistoryTable = window.document.getElementById("HistoryTable");

    luser =  window.document.getElementById("luser");
    money =  window.document.getElementById("money");
    uname = window.document.getElementById("uname");
    pass = window.document.getElementById("pass");

    tmoney =  window.document.getElementById("tmoney");
    tfrom =  window.document.getElementById("tfrom");
    tto =  window.document.getElementById("tto");
    TransferMessage = window.document.getElementById("TransferMessage");



    Trans_Amount_Span = window.document.getElementById("TransAmount");
    Trans_To_Span = window.document.getElementById("TransTo");
    Trans_Amount_Input = window.document.getElementById("amount");
    Trans_To_Input = window.document.getElementById("touname");

    transamount = 0;
    transto = "no one";





    Hide_All();
    Login_Form.style.display = "initial";
    //meta = window.document.getElementById("meta");

    //var width = new Number();
    //width = window.innerWidth;
    //width = width*1.1;    //meta.content = "width="+width.toFixed(0)+", initial-scale=1";
    //message(width.toFixed(0));
}

function Hide_All(){
    Login_Form.style.display = "none";
    UserStuff.style.display = "none";
    Transfer_Form.style.display = "none";
    Menu.style.display = "none";
    TransactionHistory.style.display = "none";
    OtherUsers.style.display = "none";
    ConfirmTransfer.style.display = "none";
    TransferMessage.style.display = "none";
    message("");
}

function message(txt){
    var msgdiv = window.document.getElementById("Message");
    msgdiv.innerHTML = txt;
}

function Login(){
    var url = "/api/login/?user="+uname.value+"&pass="+pass.value;
    //message(url);
    LoginRequest = new XMLHttpRequest();
    LoginRequest.onreadystatechange=Login_Return;
    LoginRequest.open("GET",url,true);
    LoginRequest.send();
}

function Login_Return(){
    if (LoginRequest.readyState==4){
	Login_Form.style.display = "none";
	if(LoginRequest.status==200){

	    var rv = JSON.parse(LoginRequest.responseText);
	    my_super_sekkrit_number = rv.seed;
	    Hide_All();
	    //message(LoginRequest.responseText);
	    UserStuff.style.display = "initial";
	    money.innerHTML = rv.money;
	    luser.innerHTML = uname.value;
	    Menu.style.display = "initial";
	}
	else if(LoginRequest.status==500||LoginRequest.status==400){
	    message(LoginRequest.responseText);
	}
	else{
	    message("HACKER!!");
	}
    }
}

function Logout(){
    my_super_sekkrit_number = 0;
    Hide_All();
    Login_Form.style.display = "initial";
    uname.value = "";
    pass.value = "";
}

function Transfer(){
    transamount = Trans_Amount_Input.value;
    transto = Trans_To_Input.value;
    Hide_All();
    Menu.style.display = "initial";
    ConfirmTransfer.style.display = "initial";
    Trans_Amount_Span.innerHTML = transamount;
    Trans_To_Span.innerHTML = transto;



}

function TransferCancel(){
    Display_Transfer_Form();

}

function TransferConfirm(){

    var url = "/api/send/?from="+uname.value+"&to="+transto+"&seed="+my_super_sekkrit_number+"&value="+transamount;
    TransferRequest =  new XMLHttpRequest();
    TransferRequest.onreadystatechange=Transfer_Return;
    TransferRequest.open("GET",url,true);
    TransferRequest.send();


}



function Transfer_Return(){
    if (TransferRequest.readyState==4){
	if(TransferRequest.status==200){
	    Hide_All();
	    Menu.style.display = "initial";
	    var rv = JSON.parse(TransferRequest.responseText);
	    TransferMessage.style.display = "initial";
	    tmoney.innerHTML = rv.money;
	    tfrom.innerHTML = rv.from;
	    tto.innerHTML = rv.to;
//	    message(rv.money+" kredits have been transfered from "+rv.from+" to "+rv.to);
	}
	else if(TransferRequest.status==500||TransferRequest.status==400){
	    message(TransferRequest.responseText);
	}
	else{
	    message("HACKER!!");
	}
    }
}

function Display_Transfer_Form(){
    Hide_All();
    Transfer_Form.style.display = "initial";
    Menu.style.display = "initial";
}

function Get_Transaction_History(){
    var url = "/api/history/?user="+uname.value+"&seed="+my_super_sekkrit_number;
    HistoryRequest =  new XMLHttpRequest();
    HistoryRequest.onreadystatechange=History_Return;
    HistoryRequest.open("GET",url,true);
    HistoryRequest.send();
}

function History_Return() {
    if (HistoryRequest.readyState==4) {
	    if(HistoryRequest.status==200){
    	    Hide_All();
    	    Menu.style.display = "initial";
    	    TransactionHistory.style.display = "initial";
    	    message(HistoryRequest.responseText);
    	    var rv = JSON.parse(HistoryRequest.responseText);
    	    while(HistoryTable.rows.length>1){
    		    HistoryTable.deleteRow(1);
    	    }
    	    for(k=0;k<rv.length;k++){
    		var row = HistoryTable.insertRow(HistoryTable.rows.length);
    		var date = new Number();
    		date = rv.history[k].at;
    		var datecell = row.insertCell(0);
    		datecell.innerHTML =rv.history[k].at;
    		var fromcell = row.insertCell(1);
    		fromcell.innerHTML = rv.history[k].from;
    		var tocell = row.insertCell(2);
    		tocell.innerHTML = rv.history[k].to;
    		var moneycell = row.insertCell(3);
    		moneycell.innerHTML = "<b>"+rv.history[k].money+"</b>";
    		if(rv.history[k].from == uname.value){
    		    moneycell.className = "debit";
    		}
    		if(rv.history[k].to == uname.value){
    		    moneycell.className = "credit";
    		}
	    }
	}
	else if(HistoryRequest.status==500||HistoryRequest.status==400){
	    message(HistoryRequest.responseText);
	}
	else{
	    message("HACKER!!");
	}
    }
}
