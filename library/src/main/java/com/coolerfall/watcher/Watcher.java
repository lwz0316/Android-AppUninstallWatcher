package com.coolerfall.watcher;

import android.content.Context;
import android.text.TextUtils;
import android.util.Log;

import org.json.JSONObject;

import java.io.File;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;
import java.util.Set;

/**
 * App uninstall watcher.
 *
 * @author Vincent Cheung
 * @since  Jan. 22, 2015
 */
public class Watcher {
	private static final String TAG = Watcher.class.getSimpleName();

	private static final String WATCHER_BIN_NAME = "watcher";

	private static void start(Context context, String url, File urlFile, Map<String, String> headers, Map<String, Object> httpPostParams, boolean shouldOpenBrowser) {
		String cmd = context.getDir(Command.BIN_DIR_NAME, Context.MODE_PRIVATE)
				.getAbsolutePath() + File.separator + WATCHER_BIN_NAME;

		StringBuilder cmdBuilder = new StringBuilder();
		cmdBuilder.append(cmd);
		cmdBuilder.append(" -p ");
		cmdBuilder.append(context.getPackageName());
		cmdBuilder.append(" -b ");
		cmdBuilder.append(shouldOpenBrowser ? 1 : 0);

		if (!TextUtils.isEmpty(url)) {
			cmdBuilder.append(" -u ");
			cmdBuilder.append(url);
		}

		if (urlFile != null) {
			cmdBuilder.append(" -f ");
			cmdBuilder.append(urlFile.getAbsolutePath());
		}

        if (headers != null) {
            Set<String> keys = headers.keySet();
            for (String key : keys) {
                cmdBuilder.append(" -H ");
                cmdBuilder.append(key + ":" + headers.get(key));
            }
        }

        if (httpPostParams != null) {
            String paramsData = null;
            if (headers.containsKey("Content-Type")
                    && headers.get("Content-Type").equalsIgnoreCase("application/json")) {
                try {
                    JSONObject json = new JSONObject(httpPostParams);
                    paramsData = json.toString();
                } catch (Exception e) {
                    e.printStackTrace();
                }
            } else {
                StringBuilder paramsBuilder = new StringBuilder();
                Set<String> keys = httpPostParams.keySet();
                for (String key : keys) {
                    paramsBuilder.append(key).append("=").append(httpPostParams.get(key)).append("&");
                }
                paramsData = paramsBuilder.substring(0, paramsBuilder.length() - 1);
                paramsBuilder.setLength(0);
                paramsBuilder = null;
            }
            if (!TextUtils.isEmpty(paramsData)) {
                cmdBuilder.append(" -P ");
                cmdBuilder.append(paramsData);
            }
        }

		try {
			Runtime.getRuntime().exec(cmdBuilder.toString()).waitFor();
		} catch (IOException | InterruptedException e) {
			Log.e(TAG, "start daemon error: " + e.getMessage());
		}
	}

	/**
	 * Run uninstallation watcher.
	 *
	 * @param context           context
	 * @param url               the url to gather uninstallation information
	 * @param shouldOpenBrowser should the wathcer open browser or not
	 */
	private static void run(final Context context, final String url,
	                       final File urlFile, final Map<String, String> headers, final Map<String, Object> httpPostParams, final boolean shouldOpenBrowser) {
		new Thread(new Runnable() {
			@Override
			public void run() {
				Command.install(context, WATCHER_BIN_NAME);
				start(context, url, urlFile, headers, httpPostParams, shouldOpenBrowser);
			}
		}).start();
	}

    public static class Executor {
        private Context context;
        private String url;
        private File urlFile;
        private Map<String, String> headers;
        private Map<String, Object> httpPostParams;
        private boolean shouldOpenBrowser;

        public Executor(Context context) {
            this.context = context;
        }

        public Executor url(String url) {
            this.url = url;
            return this;
        }

        public Executor urlFile(File urlFile) {
            this.urlFile = urlFile;
            return this;
        }

        public Executor addHeader(String key, String value) {
            if (this.headers == null) {
                this.headers = new HashMap<>(4);
            }
            this.headers.put(key, value);
            return this;
        }

        public Executor addHeaders(Map<String, String> headers) {
            if (this.headers == null) {
                this.headers = new HashMap<>(4);
            }
            if (headers != null) {
                this.headers.putAll(headers);
            }
            return this;
        }

        public Executor addPostParam(String key, Object value) {
            if (httpPostParams == null) {
                httpPostParams = new HashMap<>(4);
            }
            httpPostParams.put(key, value);
            return this;
        }

        public Executor addPostParams(Map<String, Object> params) {
            if (this.httpPostParams == null) {
                this.httpPostParams = new HashMap<>(4);
            }
            if (params != null) {
                this.httpPostParams.putAll(params);
            }
            return this;
        }

        public Executor shouldOpenBrowser(boolean shouldOpenBrowser) {
            this.shouldOpenBrowser = shouldOpenBrowser;
            return this;
        }

        public void execute() {
            Watcher.run(context, url, urlFile, headers, httpPostParams, shouldOpenBrowser);
        }
    }

}
