package com.coolerfall.app;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;

import com.coolerfall.watcher.Watcher;

/**
 * Created by liuwenzhu on 2016/4/26.
 */
public class BootCompletedReceiver extends BroadcastReceiver {

    private static final String URL = "http://www.baidu.com";

    @Override
    public void onReceive(Context context, Intent intent) {
        if (Intent.ACTION_BOOT_COMPLETED.equals(intent.getAction())) {
            new Watcher.Executor(context)
                    .url(URL)
                    .addPostParam("Hello", "world")
                    .addPostParam("nihao", "ppdai")
                    .addHeader("Context-Type", "application/x-www-form-urlencoded")
                    .addHeader("X_PPD_APPID", "1aabac6d068eef6a7bad3fdf50a05cc8")
                    .addHeader("User-Agent", "Android-ACTION-UNINSTALL")
                    .shouldOpenBrowser(true)
                    .execute();
        }
    }

}
