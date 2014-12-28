

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
}

function Hide_All(){
    Login_Form.style.display = "none";
    UserStuff.style.display = "none";
    Transfer_Form.style.display = "none";
    Menu.style.display = "none";
    TransactionHistory.style.display = "none";
    OtherUsers.style.display = "none";
    ConfirmTransfer.style.display = "none";
}



function Login(){
    var url = "/api/login/?user="+uname.value+"&pass="+pass.value;
    Message.innerHTML = url;
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
	    Message.innerHTML = "";
	    money.innerHTML = money;
	    luser.innerHTML = uname.value;
	    Menu.style.display = "initial";
	}
	else if(LoginRequest.status==500||LoginRequest.status==400){
	    Message.innerHTML = LoginRequest.responseText;
	}
	else{
	    Message.innerHTML="HACKER!!";
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
}

function TransferConfirm(){

var url = "";
TransferRequest =  new XMLHttpRequest();

}



function TransferReturn(){

}

function Display_Transfer_Form(){
Hide_All();
Menu.style.display = "initial";
Transfer_Form.style.display = "initial";

}
