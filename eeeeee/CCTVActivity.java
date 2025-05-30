package com.example.eeeeee;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.widget.Button;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;



public class CCTVActivity extends AppCompatActivity {
    private static final String TAG = "CCTVActivity";
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_cctv);

        // Toolbar 설정
        Toolbar toolbar = findViewById(R.id.toolbar);
        if (toolbar == null) {
            Log.e(TAG, "Toolbar is null in CCTVActivity");
            finish();
            return;
        }
        setSupportActionBar(toolbar);
        if (getSupportActionBar() != null) {
            getSupportActionBar().setDisplayHomeAsUpEnabled(true);
        } else {
            Log.e(TAG, "SupportActionBar is null");
        }

        // CCTV 보기 버튼 설정
        Button viewCctvButton = findViewById(R.id.button_view_cctv);
        if (viewCctvButton == null) {
            Log.e(TAG, "CCTV button is null. Check R.id.button_view_cctv in activity_cctv.xml");
            finish();
            return;
        }
        viewCctvButton.setOnClickListener(v -> {
            try {
                Log.d(TAG, "CCTV view button clicked, starting ActivityCctvWebView");
                startActivity(new Intent(this, CCTVWebviewActivity.class));
            } catch (Exception e) {
                Log.e(TAG, "Failed to start ActivityCctvWebView", e);
                Toast.makeText(this, "CCTV WebView 시작 실패", Toast.LENGTH_SHORT).show();
            }
        });
    }


    @Override
    public boolean onOptionsItemSelected(android.view.MenuItem item) {
        if (item.getItemId() == android.R.id.home) {
            Log.d(TAG, "Back button pressed, returning to MainActivity");
            finish(); // MainActivity로 돌아감
            return true;
        }
        return super.onOptionsItemSelected(item);
    }
}