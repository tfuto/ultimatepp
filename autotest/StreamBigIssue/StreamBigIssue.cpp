#include <Core/Core.h>

using namespace Upp;

CONSOLE_APP_MAIN
{
	StdLogSetup(LOG_COUT|LOG_FILE);
	
	String data;
	for(int i = 0; i < 40000000; i++)
		data.Cat(Random(255));
	{
		StringStream ss;
		ss.Put(data);
		ss.SetLoading();
		ss.Seek(0);
		
		String d2 = ss.GetAll(40000000);
		ASSERT(d2 == data);
		
		ss.Seek(0);
		d2 = ss.GetAll(40000001);
		ASSERT(d2.IsVoid());
	}
	
	for(int i = 0; i < 100; i++)
		data.Cat(5);

	for(int pass = 0; pass < 2; pass++)
	{
		{
			StringStream ss;
			ss % data;
			WString hh = data.ToWString();
			ss % hh;
			
			ss.SetLoading();
			ss.Seek(0);
			
			String h;
			WString wh;
	
			ss % h;
			ss % wh;
			
			ASSERT(h == data && wh == data.ToWString());
		}

		{
			StringStream ss;
			ss / data;
			
			ss.SetLoading();
			ss.Seek(0);
			
			String h;
	
			ss / h;
			
			ASSERT(h == data);
		}
		
		data = "Something smaller";
	}

	LOG("============= OK");
}
