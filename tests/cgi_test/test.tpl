blabla <(include "menu.tpl")> HIHI <(include $main)>

<(> not )> parsed  <)>

<(* if statement *)>

		<(if $test!=hoho )>
			<(print $test)>
		<(else)>
			NULL
		<(fi)>

<(if $test )>
	blabla
	<(if $test==hoho )>
		HOHO
	<(fi)>
	<(if $test==hihi )>
		HIHI
	<(fi)>	
	
	<(if $test )>
		blabla
		<(if $test==hoho )>
			<(print $test)>
		<(else)>
			NULL
		<(fi)>
	<(else)>	
		<(if $test!=hihi )>
			HOHO
		<(fi)>	
		<(if false )>
			WTF?
		<(fi)>
	<(fi)>	
	
	<(if 3>2 )>
		3>2
	<(fi)>

	<(if 3<2 )>
		3<2
	<(fi)>
	
	<(if 3!2 )>
		3!2
	<(fi)>
	
	<(
		if
		! `false`
	)>
		OK!
	<(fi)>
	
	<(if!false)>OK!<(fi)>
	
	<(if ! 'false' )>
		OK!
	<(fi)>		
	<(if ! "false" )>
		OK!
	<(fi)>	
<(fi)>


<(for i in $arr)>
	<div class="test"><(print $i)></div>
<(rof)>