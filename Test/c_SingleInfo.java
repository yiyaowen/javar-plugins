package com.yiyaowen.javar;

public class c_SingleInfo
{
    //////////////
    // Property //
    //////////////

    protected int start[];
    protected int end[];
    protected int count;

    /////////////////
    // Constructor //
    /////////////////

    public c_SingleInfo()
    {
        start = new int[1000];
        end = new int[1000];
        count = 0;
    }


    ////////////
    // setter //
    ////////////

    public void setStartAt(int s, int i)
    {
        start[i] = s;
    }
    public void setEndAt(int e, int i)
    {
        end[i] = e;
    }
    public void setCount(int c)
    {
        count = c;
    }
}
