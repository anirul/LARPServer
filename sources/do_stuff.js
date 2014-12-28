

function Login_Page_Loaded(){
    my_super_sekkrit_number = 0;

    Login_Form = window.document.getElementById("LoginForm");
    UserStuff = window.document.getElementById("UserStuff");
    Transfer_Form = window.document.getElementById("TransferForm");
    Menu = window.document.getElementById("Menu");
    TransactionHistory = window.document.getElementById("TransactionHistory");
    OtherUsers = window.document.getElementById("OtherUsers");
    ConfirmTransfer = window.document.getElementById("ConfirmTransfer");

    luser =  window.document.getElementById("luser");
    money =  window.document.getElementById("money");
    uname = window.document.getElementById("uname");
    pass = window.document.getElementById("pass");
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
    message("");
}

function message(txt){
    var msgdiv = window.document.getElementById("Message");
    msgdiv.innerHTML = txt;
}

function Login(){
    var url = "/api/login/?user="+uname.value+"&pass="+pass.value;
    message(url);
    LoginRequest = new XMLHttpRequest();
    LoginRequest.onreadystatechange=Login_Return;
    LoginRequest.open("GET",url,true);
    LoginRequest.send();
}

function Login_Return(){
    if (LoginRequest.readyState==4){
	Login_Form.style.display = "none";
	if(LoginRequest.status==200){
	    var resp = new String();
	    resp = LoginRequest.responseText;
	    var MA = resp.match(/\"seed\":([0-9]+),\"money\":([0-9]+)/);
	    //alert(MA[0]+" >"+MA[1]+"< >"+MA[2]+"<");
	    my_super_sekkrit_number = MA[1];
	    money = MA[2];
	    Hide_All();
	    UserStuff.style.display = "initial";
	    money.innerHTML = money;
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

    var url = "\api\send\?from="+uname.value+"&to="+transto+"&seed="+my_super_sekkrit_number;
    TransferRequest =  new XMLHttpRequest();
    TransferRequest.onreadystatechange=Transfer_Return;
    TransferRequest.open("GET",url,true);
    TransferRequest.send();


}



function TransferReturn(){
    if (TransferRequest.readyState==4){
	if(TransferRequest.status==200){
	    Hide_All();
	    Menu.style.display = "initial";
	    message(TransferRequest.responseText);
	}
	else if(LoginRequest.status==500||LoginRequest.status==400){
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
