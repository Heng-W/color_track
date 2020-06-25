package com.hw.colortrack;

import android.os.*;
import android.app.Activity;
import android.support.v4.app.Fragment;
import android.util.*;
import android.text.*;
import android.graphics.*;
import android.view.View.*;
import android.view.*;
import android.widget.TextView.*;
import android.widget.*;
import android.widget.LinearLayout.LayoutParams;
import android.graphics.drawable.*;
import android.widget.AdapterView.*;
import android.content.Context;

import java.util.*;
import java.util.concurrent.*;
import java.text.*;
import android.content.*;

public class SecondFragment extends Fragment {
    private View view;
    private MainActivity mActivity;
    private Client client;

    private Toast toast;

    private TextView color1Tv, color2Tv;
    private EditText et;
    private int selColorIdx;
    private byte[] rgb = new byte[3];
    private boolean isFreeDefined;

    private Handler mHandler;
    private Handler fHandler = new Handler() {

        @Override
        public void handleMessage(Message msg) {
            // TODO: Implement this method
            switch (msg.what) {
                case 0:
                    if (!client.isFreeDefined) {
                        color1Tv.setTextColor(client.getColorDefined());
                        color1Tv.setText("摄像头追踪颜色:0x" + getRgbHexString(client.rgbHex2intarr(client.colorDefined[client.colorIdx])));
                        color2Tv.setTextColor(client.getColorDefined());
                        color2Tv.setText("当前选择颜色:0x" + getRgbHexString(client.rgbHex2intarr(client.colorDefined[client.colorIdx])));

                    } else {
                        color1Tv.setTextColor(client.getColorFreeDefined());
                        color1Tv.setText("摄像头追踪颜色:0x" + getRgbHexString(client.setRgb));

                        color2Tv.setTextColor(client.getColorFreeDefined());
                        color2Tv.setText("当前选择颜色:0x" + getRgbHexString(client.setRgb));
                    }
                    isFreeDefined = client.isFreeDefined;

                    break;

                case 1:
                    if (!client.isFreeDefined) {
                        color1Tv.setTextColor(client.getColorDefined());
                        color1Tv.setText("摄像头追踪颜色:0x" + getRgbHexString(client.rgbHex2intarr(client.colorDefined[client.colorIdx])));

                    } else {
                        color1Tv.setTextColor(client.getColorFreeDefined());
                        color1Tv.setText("摄像头追踪颜色:0x" + getRgbHexString(client.setRgb));
                    }
                    break;
                case 21:
                    color1Tv.setTextColor(client.getColorFreeDefined());
                    color1Tv.setText("摄像头追踪颜色:0x" + getRgbHexString(client.setRgb));


                    break;
                case 22:
                    color1Tv.setTextColor(client.getColorDefined());
                    color1Tv.setText("摄像头追踪颜色:0x" + getRgbHexString(client.rgbHex2intarr(client.colorDefined[client.colorIdx])));

                    toast.setText("设定成功(预定义颜色)");
                    toast.show();
                    break;
                case 23:
                    color1Tv.setTextColor(client.getColorFreeDefined());
                    color1Tv.setText("摄像头追踪颜色:0x" + getRgbHexString(client.setRgb));


                    toast.setText("设定成功(自定义颜色)");
                    toast.show();
                    break;

                default:
                    break;
            }
            super.handleMessage(msg);
        }

    };


    @Override
    public void onAttach(Activity activity) {
        // TODO: Implement this method
        super.onAttach(activity);
        mActivity = (MainActivity)activity;
        client = mActivity.getClient();
        mHandler = mActivity.getHandler();
        client.setHandler(fHandler, 1);

    }


    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {

        view = inflater.inflate(R.layout.second_tab, container, false);
        return view;
    }

    @Override
    public void onActivityCreated(Bundle savedInstanceState) {
        // TODO: Implement this method
        super.onActivityCreated(savedInstanceState);
        toast = Toast.makeText(getActivity(), "", Toast.LENGTH_SHORT);
        initView();

    }

    private void initView() {

        color1Tv = (TextView)view.findViewById(R.id.color1_tv);
        color2Tv = (TextView)view.findViewById(R.id.color2_tv);

        Message msg = new Message();
        msg.what = 0;
        fHandler.sendMessage(msg);

        Button chgBtn = (Button)view.findViewById(R.id.chg_btn);
        chgBtn.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                if (!isFreeDefined) {
                    byte[] arg = new byte[1];
                    arg[0] = (byte)(selColorIdx & 0xff);
                    client.sendCmd(Command.setcolor, arg);
                } else {
                    client.sendCmd(Command.defcolor, rgb);

                }
            }
        });

        Button hfBtn = (Button)view.findViewById(R.id.hf_btn);
        hfBtn.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                for (int i = 0; i < 3; i++) {
                    rgb[i] = (byte)client.setRgb[i];
                }
                selColorIdx = client.colorIdx;


                if (client.isFreeDefined) {

                    color2Tv.setTextColor(Color.rgb(byte2int(rgb[0]), byte2int(rgb[1]), byte2int(rgb[2])));
                    color2Tv.setText("当前选择颜色:0x" + getRgbHexString(rgb));

                } else {
                    color2Tv.setTextColor(client.rgbHex2color(client.colorDefined[selColorIdx]));
                    color2Tv.setText("当前选择颜色:0x" + getRgbHexString(client.rgbHex2intarr(client.colorDefined[selColorIdx])));

                }
                isFreeDefined = client.isFreeDefined;


                toast.setText("恢复成功");
                toast.show();
            }
        });

        et = (EditText)view.findViewById(R.id.color_hex);
        et.addTextChangedListener(new TextWatcher() {

            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {

            }

            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {

            }

            @Override
            public void afterTextChanged(Editable s) {


                if (s == null || s.length() < 6) {

                    et.setTextColor(Color.BLACK);

                    return;
                }
                String colorHex = s.toString();


                for (int i = 0; i < 3; i++) {
                    rgb[i] = (byte)Integer.parseInt(colorHex.substring(2 * i, 2 * i + 2), 16);

                }
                color2Tv.setTextColor(Color.parseColor("#" + colorHex));
                color2Tv.setText("当前选择颜色:0x" + getRgbHexString(rgb));
                et.setTextColor(Color.parseColor("#" + colorHex));
                isFreeDefined = true;

            }

        });

        ImageView img[] = new ImageView[4];
        img[0] = (ImageView)view.findViewById(R.id.img1);
        img[1] = (ImageView)view.findViewById(R.id.img2);
        img[2] = (ImageView)view.findViewById(R.id.img3);
        img[3] = (ImageView)view.findViewById(R.id.img4);
        for (int i = 0; i < 4; i++) {
            img[i].setColorFilter(client.rgbHex2color(client.colorDefined[i]));
            img[i].setOnClickListener(new ImageOnClickListener(i));
        }

        ColorPicker picker = (ColorPicker) view.findViewById(R.id.picker);
        picker.setIndicatorColor(Color.WHITE);
        picker.setOrientation(ColorPicker.Orientation.HORIZONTAL);

        picker.setOnColorPickerChangeListener(new ColorPicker.OnColorPickerChangeListener() {
            @Override
            public void onColorChanged(ColorPicker picker, int color) {
                // TODO
                isFreeDefined = true;
                rgb = color2bytearr(color);
                color2Tv.setTextColor(color);
                color2Tv.setText("当前选择颜色:0x" + getRgbHexString(rgb));

            }

            @Override
            public void onStartTrackingTouch(ColorPicker picker) {
                // TODO

            }

            @Override
            public void onStopTrackingTouch(ColorPicker picker) {
                // TODO

            }
        });

    }


    private class ImageOnClickListener implements OnClickListener {
        private int idx;
        private int color;

        public ImageOnClickListener(int idx) {
            this.idx = idx;
            color = client.rgbHex2color(client.colorDefined[idx]);

        }

        @Override
        public void onClick(View p1) {
            // TODO: Implement this method
            isFreeDefined = false;
            selColorIdx = idx;
            color2Tv.setTextColor(color);
            color2Tv.setText("当前选择颜色:0x" + getRgbHexString(client.rgbHex2intarr(client.colorDefined[idx])));
        }



    }

    public int byte2int(byte b) {
        return b & 0xff;
    }

    private String getRgbHexString(byte[] b) {
        String str = "";
        for (int i = 0; i < 3; i++) {
            String tmp = Integer.toHexString(b[i] & 0xff);
            if (tmp.length() == 1) {
                tmp = "0" + tmp;
            }
            str += tmp;
        }
        return str;
    }

    private String getRgbHexString(int[] val) {
        String str = "";
        for (int i = 0; i < 3; i++) {
            String tmp = Integer.toHexString(val[i]);
            if (tmp.length() == 1) {
                tmp = "0" + tmp;
            }
            str += tmp;
        }
        return str;
    }

    public byte[] color2bytearr(int color) {
        byte[] rgb = new byte[3];
        rgb[0] = (byte)((color & 0xff0000) >> 16);
        rgb[1] = (byte)((color & 0x00ff00) >> 8);
        rgb[2] = (byte)(color & 0xff);
        return rgb;
    }

}
