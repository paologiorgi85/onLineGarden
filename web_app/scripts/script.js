function openValve(){
	alert('Open valve');
	$.get( "update_state.php", { 
		command: "1"} );
}

function closeValve(){

	alert('Close valve');
	$.get( "update_state.php", { 
		command: "0"} );
}