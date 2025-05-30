package com.example.eeeeee;

import android.annotation.SuppressLint;
import android.os.Bundle;
import android.util.Log;
import android.webkit.WebResourceError;
import android.webkit.WebResourceRequest;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;

public class CCTVWebviewActivity extends AppCompatActivity {
    private static final String TAG = "CCTVWebviewActivity";
    private static final String CCTV_URL = "http://10.10.10.106:5000";
    private WebView webView;

    @SuppressLint("SetJavaScriptEnabled")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_cctv_webview);

        // Toolbar 설정
        Toolbar toolbar = findViewById(R.id.toolbar);
        if (toolbar == null) {
            Log.e(TAG, "Toolbar is null in ActivityCctvWebView");
            finish();
            return;
        }
        setSupportActionBar(toolbar);
        if (getSupportActionBar() != null) {
            getSupportActionBar().setDisplayHomeAsUpEnabled(true);
            getSupportActionBar().setTitle("CCTV 모니터링");
        } else {
            Log.e(TAG, "SupportActionBar is null");
        }

        // WebView 설정
        webView = findViewById(R.id.webview_cctv);
        if (webView == null) {
            Log.e(TAG, "WebView is null in ActivityCctvWebView");
            finish();
            return;
        }

        setupWebView();
        loadCctvStream();
    }

    @SuppressLint("SetJavaScriptEnabled")
    private void setupWebView() {
        WebSettings webSettings = webView.getSettings();
        webSettings.setJavaScriptEnabled(true);
        webSettings.setLoadWithOverviewMode(true);
        webSettings.setUseWideViewPort(true);
        webSettings.setDomStorageEnabled(true);
        webSettings.setMediaPlaybackRequiresUserGesture(false); // 자동 재생을 위해 필요할 수 있음

        webView.setWebViewClient(new WebViewClient() {
            @Override
            public void onPageFinished(WebView view, String url) {
                Log.d(TAG, "CCTV stream loaded: " + url);
            }

            @Override
            public void onReceivedError(WebView view, WebResourceRequest request, WebResourceError error) {
                Log.e(TAG, "WebView error: URL: " + request.getUrl());
                runOnUiThread(() -> {
                    Toast.makeText(CCTVWebviewActivity.this,
                            "CCTV 로드 실패: 네트워크 연결을 확인해주세요",
                            Toast.LENGTH_LONG).show();
                });
            }
        });
    }

    private void loadCctvStream() {
        try {
            webView.loadUrl(CCTV_URL);
        } catch (Exception e) {
            Log.e(TAG, "Failed to load CCTV URL", e);
            Toast.makeText(this, "CCTV URL 로드 실패", Toast.LENGTH_SHORT).show();
        }
    }

    @Override
    public boolean onOptionsItemSelected(android.view.MenuItem item) {
        if (item.getItemId() == android.R.id.home) {
            Log.d(TAG, "Toolbar back button pressed, returning to CCTVActivity");
            finish(); // CCTVActivity로 돌아감
            return true;
        }
        return super.onOptionsItemSelected(item);
    }

    @Override
    public void onBackPressed() {
        if (webView != null && webView.canGoBack()) {
            Log.d(TAG, "WebView back pressed, navigating to previous page");
            webView.goBack(); // WebView의 이전 페이지로 이동
        } else {
            Log.d(TAG, "Device back button pressed, returning to CCTVActivity");
            super.onBackPressed(); // CCTVActivity로 돌아감
        }
    }

    @Override
    protected void onDestroy() {
        if (webView != null) {
            webView.destroy();
        }
        super.onDestroy();
    }
}