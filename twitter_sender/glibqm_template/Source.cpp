#include <base.h>
#include <mine.h>
#include <net.h>

#include <qminer.h>
#include <qminer_srv.h>
#include <qminer_gs.h>


class TWebPgPost : public TWebPgFetch {
private:
	TInt ErrorCount, OkCount;
public:
	PWebPg WebPg;
	TWebPgPost() : TWebPgFetch() {};
	void OnFetch(const int& FId, const PWebPg& _WebPg){
		OkCount++;
		//printf("OK:%d, ERR: %d\n", OkCount, ErrorCount);
		WebPg=_WebPg; 
		if (WebPg->GetHttpResp()->GetStatusCd() != 200){
			printf("%s\n%s\n",  WebPg->GetHttpResp()->GetHdStr().CStr(), WebPg->GetHttpResp()->GetBodyAsStr().CStr());
		}
	}
	void OnError(const int& FId,  const TStr& MsgStr) {
		printf("%s\n", MsgStr.CStr());
		ErrorCount++;
		//printf("OK:%d, ERR: %d\n", OkCount, ErrorCount);		
	}
};

void SendToServer(TWebPgPost& Fetch, const TStr& UrlStr, const TStr& LineStr){
	TStr BodyStr = LineStr;
	TMem BodyMem; BodyMem.AddBf(BodyStr.CStr(), BodyStr.Len());
	PHttpRq HttpRq = THttpRq::New(hrmPost, TUrl::New(UrlStr), THttp::TextXmlFldVal, BodyMem); //TextFldVal
	
	//TStr tesi = HttpRq->GetStr();
	
	
	Fetch.FetchHttpRq(HttpRq);	
	
};

void ProcessTweetJSON(const TStr& InputPath, const TStr& UrlStr){
		
	PSIn FIn = TZipIn::NewIfZip(InputPath);
	TTm TmUTC;	
	TStr LineStr;

	TWebPgPost Fetch;
	Fetch.PutMxConns(10);
		
	TUInt64 TweetN = 0;
	while (!FIn->Eof()){
		FIn->GetNextLn(LineStr);
		TweetN++;
		if (TweetN % 1000 == 0){
			printf("%d\r", TweetN);
		}
		//"author":{"Username":"forexliveexpert"}, "Date":"2011-08-01T00:00:11", "Text":"kamaleon make 44 pips with audjpy [short] #audjpy: opened 26\/07 17:35 and closed 01\/08 02:34? 
		PJsonVal TweetVal = TJsonVal::GetValFromStr(LineStr);
		if (TweetVal->IsDef()){
			TweetVal->GetObjKey("ID")->PutStr(TGuid::GenGuid());			
			SendToServer(Fetch,   UrlStr, TJsonVal::GetStrFromVal(TweetVal)); //todo
		}
	}
	TLoop::Run();
}


int main(int argc, char* argv[]) {
	
	Env = TEnv(argc, argv, TNotify::StdNotify);
	// command line parameters
	Env.PrepArgs("Twitter sender", 0);
	int PortN = Env.GetIfArgPrefixInt("-port:", 52670, "Port");
	TStr FunNm = Env.GetIfArgPrefixStr("-function:", "addtweet", "Function name");
	TStr ServerNm = Env.GetIfArgPrefixStr("-server:", "http://localhost", "Server url");	

	//ProcessTweetJSON("1000tweets.txt",  ServerNm + ":" + TInt::GetStr(PortN) + "/" + FunNm);
	ProcessTweetJSON("1h_1percent_tweets.txt",  ServerNm + ":" + TInt::GetStr(PortN) + "/" + FunNm);
	
	return 0;
}


