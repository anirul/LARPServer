

function Login_Page_Loaded(){

}

function Login(){
    Login_Form = window.document.getElementById("LoginForm");
    stuff = window.document.getElementById("stuff");
    uname = window.document.getElementById("uname").value;
    pass = window.document.getElementById("pass").value;
    Login_Form.style.display = "none";
    var url = "/api/login/?name="+uname+"&pass="+pass;
    //stuff.innerHTML = url;
    xmlhttp=new XMLHttpRequest();
    xmlhttp.open("GET",url,true);
    xmlhttp.send();

}

function Login_Return(){
    if (xmlhttp.readyState==4){
	if(xmlhttp.status==200){
	    stuff.innerHTML("Welcome "+uname);
	}
	if(xmlhttp.status==500){
	    stuff.innerHTML(xmlhttp.responseText);
	}
	else{
	    stuff.innerHTML("faut d'erreur! Grave! Grave!!");
	}
    }
}