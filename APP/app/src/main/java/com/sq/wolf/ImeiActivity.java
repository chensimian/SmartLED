package com.sq.wolf;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

public class ImeiActivity extends AppCompatActivity {
    private EditText et_imei;
    private EditText et_ip;
    private EditText et_port;
    private Button btn_sure;
    private ACache cache;
    private void initPara() {
        cache = ACache.get(this);
    }

    private void initUI() {
        et_imei = (EditText)findViewById(R.id.et_imei);
        et_ip = (EditText)findViewById(R.id.et_ip);
        et_port = (EditText)findViewById(R.id.et_port);
        btn_sure = (Button)findViewById(R.id.btn_sure);
    }

    private void initEditText() {
        String imei = cache.getAsString("imei");
        String ip = cache.getAsString("ip");
        String port = cache.getAsString("port");
        if (imei != null) {
            et_imei.setText(imei);
            et_imei.setSelection(imei.length());
        }
        if (ip != null) {
            et_ip.setText(ip);
        }
        else {
            et_ip.setText("111.229.82.92");
        }
        et_ip.setSelection(et_ip.length());
        if (port != null) {
            et_port.setText(port);
        }
        else {
            et_port.setText("61613");
        }
        et_port.setSelection(et_port.length());
    }

    private void initButton() {
        btn_sure.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String imei = et_imei.getText().toString();
                String ip = et_ip.getText().toString();
                String port = et_port.getText().toString();
                if (imei.length() == 0) {
                    et_imei.requestFocus();
                    Toast.makeText(ImeiActivity.this, "IMEI号输入为空", Toast.LENGTH_SHORT).show();
                    return;
                }
                if (ip.length() == 0) {
                    et_ip.requestFocus();
                    Toast.makeText(ImeiActivity.this, "IP输入为空", Toast.LENGTH_SHORT).show();
                    return;
                }
                if (port.length() == 0) {
                    et_port.requestFocus();
                    Toast.makeText(ImeiActivity.this, "端口输入为空", Toast.LENGTH_SHORT).show();
                    return;
                }
                cache.put("imei", imei);
                cache.put("ip", ip);
                cache.put("port", port);
                finish();
                Intent intent = new Intent(ImeiActivity.this, MainActivity.class);
                startActivity(intent);
            }
        });
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_imei);
        initPara();
        initUI();
        initEditText();
        initButton();
    }
}
