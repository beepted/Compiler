main
begin
	int x;
	int fact;
	int tep;
	int y;
	function factorial (int) result int;

	x=5;
	tep=100;
	y=call factorial(x);

	if (y.>.tep)
	begin
		y=y+1;
	end
	if(y.<.tep)
	begin
		y=y-1;
	end	
	print y;
end

function factorial result int (int x)
begin
	int fact;
	int temp;
	temp=1;
	fact=1;
	while(x.>=.temp)
	begin
		fact = fact * x;
		x=(x-1)+2*3;
	end
	return fact;	
end
