package com.coolerfall.ui;

import android.app.Activity;
import android.os.Bundle;

import com.coolerfall.watcher.Watcher;
import com.coolerfall.watcher.sample.R;

public class MainActivity extends Activity {

	private static final String URL = "http://192.168.23.44:8888";

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);

		new Watcher.Executor(this)
				.url(URL)
				.addPostParam("Hello", "world")
				.addPostParam("nihao", "ppdai")
				.addHeader("Content-Type", "application/json")
				.addHeader("X_PPD_APPID", "1aabac6d068eef6a7bad3fdf50a05cc8")
				.addHeader("User-Agent", "Android-ACTION-UNINSTALL")
				.shouldOpenBrowser(true)
				.execute();
	}
}
