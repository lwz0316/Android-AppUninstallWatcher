Android-AppUninstallWatcher
===========================
App uninstall watcher. You can monitor uninstall of your app.

**This library forked from** [Coolerfall/Android-AppUninstallWatcher](https://github.com/Coolerfall/Android-AppUninstallWatcher), just add the supprot for http post params and http headers 

Usage
=====
* This wathcer can be added in application or service in your app, use 
	
		new Watcher.Executor(context)
				.url(URL)
				.addPostParam("hello", "world")
				.addPostParam("extra", "uninstall-watcher")
				.addHeader("Content-Type", "application/json")
				.addHeader("My-Header", "Android-ACTION-UNINSTALL")
				.shouldOpenBrowser(true)
				.execute();	

	to run the watcher.

* If headers contains `Content-Type` and the value equals to `application/json`, the http params will convert to the json string. 

		// Content-Type: application/json
		// post params will convert to the json string.
		{"hello":"world", "extra":"uninstall-watcher"}

* If `shouldOpenBrowser` is true, the wathcer will open default browser with `url`. Don't forget to add *android.permission.INTERNET* permission in manifest.
* If you want to keep your app alive, see also [Android-AppDaemon][1].

Note
====
This library dosen't work on all phones, such as xiaomi phones.


License
=======

    Copyright (C) 2015-2016 Wenzhu Liu

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

         http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

[1]: https://github.com/Coolerfall/Android-AppDaemon