package com.yiyaowen.javar;

import com.yiyaowen.javar.c_SingleInfo;

public class c_SyntaxParseInfo
{
    //////////////
    // Property //
    //////////////

    protected c_SingleInfo aKeywordInfo[];
    protected c_SingleInfo quoteInfo;
    protected c_SingleInfo commentInfo;

    /////////////////
    // Constructor //
    /////////////////

    public c_SyntaxParseInfo(int kwTotalCount)
    {
        aKeywordInfo = new c_SingleInfo[kwTotalCount];
        for (int i = 0; i < kwTotalCount; ++i) { aKeywordInfo[i] = new c_SingleInfo(); }
        quoteInfo = new c_SingleInfo();
        commentInfo = new c_SingleInfo();
    }

    ////////////
    // setter //
    ////////////

    public void setAKeywordInfoAt(c_SingleInfo kwInfo, int i)
    {
        aKeywordInfo[i] = kwInfo;
    }
    public void setQuoteInfo(c_SingleInfo qInfo)
    {
        quoteInfo = qInfo;
    }
    public void setCommentInfo(c_SingleInfo cInfo)
    {
        commentInfo = cInfo;
    }
}
