package com.example.eeeeee;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import com.example.eeeeee.R;

import org.json.JSONObject;

import java.io.IOException;

import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.MediaType;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.RequestBody;
import okhttp3.Response;
import okhttp3.logging.HttpLoggingInterceptor;

public class LoginActivity extends AppCompatActivity {
    private static final String TAG = "LoginActivity";
    private EditText editTextUsername, editTextPassword;
    private Button buttonLogin;
    private OkHttpClient client;
    private static final MediaType JSON = MediaType.parse("application/json; charset=utf-8");
    private static final String SERVER_URL = "http://10.10.10.106:5001";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.d(TAG, "onCreate called");
        setContentView(R.layout.activity_login);

        // UI 요소 초기화
        try {
            editTextUsername = findViewById(R.id.editTextUsername);
            editTextPassword = findViewById(R.id.editTextPassword);
            buttonLogin = findViewById(R.id.buttonLogin);
            Log.d(TAG, "UI elements initialized: buttonLogin ID=" + R.id.buttonLogin);
        } catch (Exception e) {
            Log.e(TAG, "Error initializing UI: " + e.getMessage(), e);
            Toast.makeText(this, "UI 초기화 오류: " + e.getMessage(), Toast.LENGTH_LONG).show();
            return;
        }

        // OkHttp 클라이언트 초기화 (로깅 인터셉터 포함)
        HttpLoggingInterceptor logging = new HttpLoggingInterceptor(message -> Log.d(TAG, message));
        logging.setLevel(HttpLoggingInterceptor.Level.BODY);
        client = new OkHttpClient.Builder()
                .addInterceptor(logging)
                .connectTimeout(30, java.util.concurrent.TimeUnit.SECONDS)
                .readTimeout(30, java.util.concurrent.TimeUnit.SECONDS)
                .writeTimeout(30, java.util.concurrent.TimeUnit.SECONDS)
                .build();

        // 로그인 버튼 클릭 리스너
        buttonLogin.setOnClickListener(v -> {
            Log.d(TAG, "Login button clicked");
            String id = editTextUsername.getText().toString().trim();
            String password = editTextPassword.getText().toString().trim();

            if (id.isEmpty() || password.isEmpty()) {
                Log.w(TAG, "Empty id or password");
                Toast.makeText(LoginActivity.this, "아이디와 비밀번호를 입력해주세요", Toast.LENGTH_SHORT).show();
                return;
            }

            // JSON 요청 본문 생성
            JSONObject jsonBody = new JSONObject();
            try {
                jsonBody.put("id", id);
                jsonBody.put("password", password);
                Log.d(TAG, "Request Body: " + jsonBody.toString());
            } catch (Exception e) {
                Log.e(TAG, "Error creating JSON: " + e.getMessage(), e);
                Toast.makeText(LoginActivity.this, "요청 생성 오류", Toast.LENGTH_SHORT).show();
                return;
            }

            // 비동기 로그인 요청
            loginRequest(jsonBody.toString());
        });
    }

    private void loginRequest(String jsonBody) {
        String url = SERVER_URL + "/login";
        RequestBody body = RequestBody.create(jsonBody, JSON);
        Request request = new Request.Builder()
                .url(url)
                .post(body)
                .addHeader("Content-Type", "application/json; charset=UTF-8")
                .build();

        client.newCall(request).enqueue(new Callback() {
            @Override
            public void onFailure(@NonNull Call call, @NonNull IOException e) {
                Log.e(TAG, "Network error", e);
                runOnUiThread(() -> Toast.makeText(LoginActivity.this, "네트워크 오류: " + e.getMessage(), Toast.LENGTH_LONG).show());
            }

            @Override
            public void onResponse(@NonNull Call call, @NonNull Response response) throws IOException {
                String responseBody = response.body() != null ? response.body().string() : "";
                Log.d(TAG, "Response: " + responseBody);

                runOnUiThread(() -> {
                    try {
                        JSONObject jsonResponse = new JSONObject(responseBody);
                        if (jsonResponse.has("message") && jsonResponse.getString("message").equals("Login successful")) {
                            String name = jsonResponse.optString("name", "");
                            String userId = jsonResponse.optString("id", "");
                            int num = jsonResponse.optInt("num", -1);

                            if (num == -1) {
                                Log.e(TAG, "Invalid num in response: " + responseBody);
                                Toast.makeText(LoginActivity.this, "서버에서 유효한 num 값을 받지 못했습니다", Toast.LENGTH_LONG).show();
                                return;
                            }

                            Toast.makeText(LoginActivity.this, "반갑습니다!! " + name, Toast.LENGTH_LONG).show();

                            // MainActivity로 이동
                            Intent intent = new Intent(LoginActivity.this, MainActivity.class);
                            intent.putExtra("USER_NAME", name);
                            intent.putExtra("USER_ID", userId);
                            intent.putExtra("num", num);
                            Log.d(TAG, "Starting MainActivity with num: " + num);
                            startActivity(intent);
                            finish();
                        } else {
                            String error = jsonResponse.optString("error", "알 수 없는 오류");
                            Log.w(TAG, "Login failed: " + error);
                            Toast.makeText(LoginActivity.this, error, Toast.LENGTH_LONG).show();
                        }
                    } catch (Exception e) {
                        Log.e(TAG, "Error parsing response", e);
                        Toast.makeText(LoginActivity.this, "응답 처리 오류", Toast.LENGTH_LONG).show();
                    }
                });
            }
        });
    }
}