#include <base.h>
#include <mine.h>
#include <net.h>


class TSASFunTest : public TSAppSrvFun {
public:
	int counter;
	TTmStopWatch Timer;
	TSASFunTest(): TSAppSrvFun("update_dg", saotJSon), Timer(true) { counter = 0;}
	TSASFunTest(const TStr& FunNm): TSAppSrvFun(FunNm, saotJSon), Timer(true) { counter = 0;}
    static PSAppSrvFun New() { return new TSASFunTest(); }
	static PSAppSrvFun New(const TStr& FunNm) { return new TSASFunTest(FunNm); }
	
	TStr ExecJSon(const TStrKdV& FldNmValPrV, const PSAppSrvRqEnv& RqEnv) {
		
		counter++;
		PHttpRq HttpRq = RqEnv->GetHttpRq();
		//if (counter == 1) {
		//	printf("\n%s\n%s\n\n", HttpRq->GetHdStr().CStr(), HttpRq->GetBodyAsStr().CStr());			
		//}
		if (Timer.GetMSec() > 1000){
			Timer.Reset(true);
			printf("%d\r", counter);
		}
		return ""; 	
	};
};


int main(int argc, char* argv[]) {
 		
	try {
		// create environment
		Env = TEnv(argc, argv, TNotify::StdNotify);
		// command line parameters
		Env.PrepArgs("Test Server", 0);
		int PortN = Env.GetIfArgPrefixInt("-port:", 52670 + 1, "Port");
		TStr FunNm = Env.GetIfArgPrefixStr("-fun_name:", "update_dg", "Function name");

		TSAppSrvFunV SrvFunV;		
		SrvFunV.Add(TSASFunTest::New(FunNm)); //fill		
		
		PWebSrv WebSrv = TSAppSrv::New(PortN, SrvFunV, TNullNotify::New(), true, true);
		//PWebSrv WebSrv = TSAppSrv::New(PortN, SrvFunV, TStdNotify::New(), true, true); 
		
		//TLoop::Ref();
		TLoop::Run();


	} catch (const PExcept& Except) {
		// GLib exception
		printf("Error: %s", Except->GetMsgStr().CStr());
		return 2;
	} catch (...) {
		// other exceptions
		return 1;
	}
	return 0;

}