

function Login_Page_Loaded(){
    my_super_sekkrit_number = 0;
    Login_Form = window.document.getElementById("LoginForm");
    stuff = window.document.getElementById("stuff");
    uname = window.document.getElementById("uname");
    pass = window.document.getElementById("pass");
    Transfer_Form = window.document.getElementById("TransferForm");
    Transfer_Form.style.display = "none";
}



function Login(){
    var url = "/api/login/?user="+uname.value+"&pass="+pass.value;
    stuff.innerHTML = url;
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
	    alert(MA[0]+" >"+MA[1]"< >"+MA[2]+"<");
	    
	    stuff.innerHTML = "Welcome "+uname.value+" you have "+LoginRequest.responseText+" kredits.";
	    Transfer_Form.style.display = "all";
	}
	else if(LoginRequest.status==500||LoginRequest.status==400){
	    stuff.innerHTML = LoginRequest.responseText;
	}
	else{
	    stuff.innerHTML="HACKER!!";
	}
    }
}


function Transfer(){

var url = "";
TransferRequest =  new XMLHttpRequest();

}

function TransferReturn(){

}

