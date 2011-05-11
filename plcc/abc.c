main
	begin
	int a;
	int b;
	int c;
	function foo(int,int) result int;
	function foo1(int,int) result int;
	a=10;
	b=a+(10*2);
	if (b.>.a)
		begin
		c = 30;
		print c;
		end
	a=call foo(a,b,c);
	c=call foo1(a,b);
	while(b.>.a)
		begin
		print b;
		end
	end

function foo result int(int a,int b)
	begin
	int e;
	int c;
	a=5;
	e=e+a;
	print e;
	return e;
	end	
function foo1 result int(int a,int b)
begin
int f;
f=a+b;
print f;
return f;
end

