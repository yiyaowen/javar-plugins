package com.yiyaowen.javar;

import com.yiyaowen.javar.c_SyntaxParseInfo;

import java.io.*;

public class c_SyntaxParser
{
    public static native void fillSyntaxParseInfo(c_SyntaxParseInfo info, String file, String keywords[], int kwTotalCount, char splitSymbols[], int ssTotalCount);

    static
    {
        System.setProperty("java.library.path", "../Lib");
        System.loadLibrary("parser");
    }

    public static void main(String[] args)
    {
        String file;

        if (args.length != 1)
        {
            System.out.println("Usage: java [package.class] filename");
            return;
        }
        try (
            var is = new FileInputStream(args[0]);
            var streamReader = new InputStreamReader(is);
            var reader = new BufferedReader(streamReader))
        {
            String line;
            var stringBuilder = new StringBuilder();
            while ((line = reader.readLine()) != null)
            {
                stringBuilder.append(line+"\n");
            }
            file = String.valueOf(stringBuilder);
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
            return;
        }
        String keywords[] = new String[] 
        { 
            "public", "static", "class", "new", "#include", "void", "int",
            "float", "double", "long", "bool", "using", "namespace"
        };
        int kwTotalCount = 13;
        char splitSymbols[] = new char[] { ';', ' ', '\n', '\t' };
        int ssTotalCount = 4;
        c_SyntaxParseInfo info = new c_SyntaxParseInfo(kwTotalCount);
        fillSyntaxParseInfo(info, file, keywords, kwTotalCount, splitSymbols, ssTotalCount);
        System.out.println("Keyword:");
        for (int i = 0; i < kwTotalCount; ++i)
        {
            System.out.println(keywords[i]);
            for (int j = 0; j < info.aKeywordInfo[i].count; ++j)
            {
                System.out.println(String.valueOf(info.aKeywordInfo[i].start[j]) +
                    ", " + String.valueOf(info.aKeywordInfo[i].end[j]));
            }
        } 
        System.out.println("\nQuote:");
        for (int i = 0; i < info.quoteInfo.count; ++i) 
        {
            System.out.println(String.valueOf(info.quoteInfo.start[i]) +
                ", " + String.valueOf(info.quoteInfo.end[i]));
        }
        System.out.println("\nComment:");
        for (int i = 0; i < info.commentInfo.count; ++i) 
        {
            System.out.println(String.valueOf(info.commentInfo.start[i]) +
                ", " + String.valueOf(info.commentInfo.end[i]));
        }
    }
}
