

function Login_Page_Loaded(){
    my_super_sekkrit_number = 0;
    Login_Form = window.document.getElementById("LoginForm");
    UserStuff = window.document.getElementById("UserStuff");
    UserStuff.style.display = "none";
    uname = window.document.getElementById("uname");
    pass = window.document.getElementById("pass");
    Transfer_Form = window.document.getElementById("TransferForm");
    Transfer_Form.style.display = "none";
    Menu = window.document.getElementById("Menu");
    Menu.style.display = "none";
    TransactionHistory = window.document.getElementById("TransactionHistory");
    TransactionHistory.style.display = "none";
    OtherUsers = window.document.getElementById("OtherUsers");
    OtherUsers.style.display = "none";
    ConfirmTransfer = window.document.getElementById("ConfirmTransfer");
    ConfirmTransfer.style.display = "none";
    luser =  window.document.getElementById("luser");
    money =  window.document.getElementById("money");
}



function Login(){
    var url = "/api/login/?user="+uname.value+"&pass="+pass.value;
    UserStuff.innerHTML = url;
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
	    UserStuff.style.display = "all";
	    Message.innerHTML = "";
	    money.innerHTML = money;
	    luser.innerHTML = uname;
	}
	else if(LoginRequest.status==500||LoginRequest.status==400){
	    Message.innerHTML = LoginRequest.responseText;
	}
	else{
	    Message.innerHTML="HACKER!!";
	}
    }
}


function Transfer(){

var url = "";
TransferRequest =  new XMLHttpRequest();

}

function TransferReturn(){

}

