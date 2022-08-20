package com.sq.wolf;

import android.content.Context;
import android.content.res.TypedArray;
import android.util.AttributeSet;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.suke.widget.SwitchButton;

/**
 * Created by chensimian.
 */

public class SwitchCustom extends LinearLayout{
    private ImageView iv_leftImage;
    private TextView tv_leftText;
    private SwitchButton sw_switch;
    private View v_top;
    private View v_bottom;
    private OnSwitchCustomSelectedListener listener;
    public interface OnSwitchCustomSelectedListener {
        public void onSelected(boolean isChecked);
    }

    public void setOnSwitchCustomSelectedListener(OnSwitchCustomSelectedListener listener) {
        this.listener = listener;
    }
    private void initUI() {
        iv_leftImage = (ImageView)findViewById(R.id.iv_leftImage);
        tv_leftText = (TextView)findViewById(R.id.tv_leftText);
        sw_switch = (SwitchButton)findViewById(R.id.sw_switch);
        v_top = (View)findViewById(R.id.v_top);
        v_bottom = (View)findViewById(R.id.v_bottom);
    }
    public SwitchCustom(Context context, AttributeSet attrs) {
        super(context, attrs);
        LayoutInflater.from(context).inflate(R.layout.switch_custom, this);
        initUI();
        TypedArray arr = context.obtainStyledAttributes(attrs, R.styleable.SwitchCustom);
        String leftText = arr.getString(R.styleable.SwitchCustom_leftText);
        int leftImage = arr.getResourceId(R.styleable.SwitchCustom_leftImage, 0);
        boolean status = arr.getBoolean(R.styleable.SwitchCustom_status, false);
        boolean topBorderVisible = arr.getBoolean(R.styleable.SwitchCustom_topBorderVisible, true);
        boolean bottomBorderVisible = arr.getBoolean(R.styleable.SwitchCustom_bottomBorderVisible, true);
        if (leftImage != 0) {
            iv_leftImage.setImageResource(leftImage);
        }
        else {
            iv_leftImage.setVisibility(View.GONE);
        }
        tv_leftText.setText(leftText != null ? leftText : "");
        sw_switch.setChecked(status);
        v_top.setVisibility(topBorderVisible ? View.VISIBLE : View.GONE);
        v_bottom.setVisibility(bottomBorderVisible ? View.VISIBLE : View.GONE);
        sw_switch.setOnCheckedChangeListener(new SwitchButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(SwitchButton view, boolean isChecked) {
                if (listener != null)
                    listener.onSelected(isChecked);
            }
        });
        arr.recycle();
    }

    public void setStatus(boolean checked) {
        sw_switch.setChecked(checked);
    }
}
