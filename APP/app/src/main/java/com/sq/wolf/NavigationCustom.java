package com.sq.wolf;

import android.content.Context;
import android.content.res.TypedArray;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

/**
 * Created by chensimian.
 */

public class NavigationCustom extends LinearLayout {
    private final static String TAG = "NavagationLayout";
    private TextView tv_leftText;
    private TextView tv_rightText;
    private ImageView iv_leftImage;
    private ImageView iv_rightImage;
    private View v_top;
    private View v_bottom;
    private LinearLayout ll_navigation;
    private OnNavigationClickListener listener = null;
    public interface OnNavigationClickListener {
        public void onClick();
    }

    public void setOnNavigationClickListener(OnNavigationClickListener listener) {
        this.listener = listener;
    }

    private void initUI() {
        tv_leftText = (TextView)findViewById(R.id.tv_leftText);
        tv_rightText = (TextView)findViewById(R.id.tv_rightText);
        iv_leftImage = (ImageView)findViewById(R.id.iv_leftImage);
        iv_rightImage = (ImageView)findViewById(R.id.iv_rightImage);
        v_top = (View)findViewById(R.id.v_top);
        v_bottom = (View)findViewById(R.id.v_bottom);
        ll_navigation =  (LinearLayout)findViewById(R.id.ll_navigation);
    }

    private void initLinearLayout() {
        ll_navigation.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                if (listener != null) {
                    listener.onClick();
                }
            }
        });
    }

    private void initTextView() {

    }

    private void initImageView() {

    }

    private void setTopBorderShow(boolean b) {
        v_top.setVisibility(View.GONE);
    }

    private void setBottomBorderShow(boolean b) {
        v_bottom.setVisibility(View.GONE);
    }


    public void setLeftText(String text) {
        tv_leftText.setText(text);
    }

    public void setRightText(String text) {
        tv_rightText.setText(text);
    }

    public void setLeftImage(int id) {
        iv_leftImage.setImageResource(id);
    }

    public void setRightImage(int id) {
        iv_rightImage.setImageResource(id);
    }

    public NavigationCustom(Context context, AttributeSet attrs) {
        super(context, attrs);
        LayoutInflater.from(context).inflate(R.layout.navigation_custom, this);
        initUI();
        initLinearLayout();
        initTextView();
        initImageView();
        TypedArray arr = context.obtainStyledAttributes(attrs, R.styleable.NavigationCustom);
        String leftText = arr.getString(R.styleable.NavigationCustom_leftText);
        String rightText = arr.getString(R.styleable.NavigationCustom_rightText);
        int leftImage = arr.getResourceId(R.styleable.NavigationCustom_leftImage, 0);
        int rightImage = arr.getResourceId(R.styleable.NavigationCustom_rightImage, 0);
        boolean topBorderVisible = arr.getBoolean(R.styleable.NavigationCustom_topBorderVisible, true);
        boolean bottomBorderVisible = arr.getBoolean(R.styleable.NavigationCustom_bottomBorderVisible, true);
        if (leftImage != 0) {
            iv_leftImage.setImageResource(leftImage);
        }
        else {
            iv_leftImage.setVisibility(View.GONE);
        }
        tv_leftText.setText(leftText != null ? leftText : "");
        if (rightImage != 0) {
            iv_rightImage.setImageResource(rightImage);
        }
        else {
            iv_rightImage.setVisibility(View.GONE);
        }
        tv_rightText.setText(rightText != null ? rightText : "");
        v_top.setVisibility(topBorderVisible ? View.VISIBLE : View.GONE);
        v_bottom.setVisibility(bottomBorderVisible ? View.VISIBLE : View.GONE);
        arr.recycle();
    }
}
