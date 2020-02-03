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

public class FirstFragment extends Fragment {

    private View view;
    private MainActivity mActivity;
    private Client client;

    private Toast toast;

    private TextView modeTv;
    private Button startBtn;


    private Handler mHandler;
    private Handler fHandler = new Handler() {

        @Override
        public void handleMessage(Message msg) {
            // TODO: Implement this method
            switch (msg.what) {
                case 0:
                //startActivityForResult(new Intent(mActivity,SettingActivity.class),1);
                case 1:
                    if (client.isStarted) {
                        modeTv.setText("当前模式:自动");
                        startBtn.setText("停止跟踪");
                    } else {
                        modeTv.setText("当前模式:手动");
                        startBtn.setText("开始跟踪");
                    }
                    break;
                case 21:
                    if (client.isStarted) {
                        modeTv.setText("当前模式:自动");
                        startBtn.setText("停止跟踪");
                    } else {
                        modeTv.setText("当前模式:手动");
                        startBtn.setText("开始跟踪");
                    }
                    toast.setText("设置成功");
                    toast.show();
                    break;
                case 22:
                    toast.setText("调整成功");
                    toast.show();
                    break;
                case 23:
                    toast.setText("宽高设置成功");
                    toast.show();
                    break;
                case 24:
                    toast.setText("图像HSL采集成功\n" + "H:" + String.valueOf(client.setHsl[0]) + " S:" + String.valueOf(client.setHsl[1]) + " L:" + String.valueOf(client.setHsl[2]));
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
        client.setHandler(fHandler, 0);
    }


    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {

        view = inflater.inflate(R.layout.first_tab, container, false);
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
        modeTv = (TextView)view.findViewById(R.id.mode);

        startBtn = (Button)view.findViewById(R.id.start_btn);

        Message msg = new Message();
        msg.what = 1;
        fHandler.sendMessage(msg);

        startBtn.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                if (!client.isStarted) {
                    client.sendCmd(Command.start);
                } else {
                    client.sendCmd(Command.stop);
                }
            }
        });

        Button upBtn = (Button)view.findViewById(R.id.up_btn);
        upBtn.setOnClickListener(new CameraClickListener(Command.up));

        Button downBtn = (Button)view.findViewById(R.id.down_btn);
        downBtn.setOnClickListener(new CameraClickListener(Command.down));

        Button leftBtn = (Button)view.findViewById(R.id.left_btn);
        leftBtn.setOnClickListener(new CameraClickListener(Command.left));

        Button rightBtn = (Button)view.findViewById(R.id.right_btn);
        rightBtn.setOnClickListener(new CameraClickListener(Command.right));

        Button sendwhBtn = (Button)view.findViewById(R.id.sendwh_btn);
        sendwhBtn.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {

                EditText etw = (EditText)view.findViewById(R.id.edit_w);
                EditText eth = (EditText)view.findViewById(R.id.edit_h);
                String wstr = etw.getText().toString();

                String hstr = eth.getText().toString();
                if (wstr == null) {
                    toast.setText("请填写宽度");
                    toast.show();
                    return;
                }
                if (hstr == null) {
                    toast.setText("请填写高度");
                    toast.show();
                    return;
                }
                int w = Integer.parseInt(wstr);
                int h = Integer.parseInt(hstr);

                if (w > 240) {
                    toast.setText("宽度过大");
                    toast.show();
                    return;
                }
                if (h > 240) {
                    toast.setText("高度过大");
                    toast.show();
                    return;
                }
                byte[] size = new byte[2];
                size[0] = (byte)(w & 0xff);
                size[1] = (byte)(h & 0xff);
                client.sendCmd(Command.setsize, size);

            }
        });

        Button pickBtn = (Button)view.findViewById(R.id.pick_btn);
        pickBtn.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                if (!client.isStarted) {
                    client.sendCmd(Command.pick);
                } else {
                    toast.setText("请先停止追踪");
                    toast.show();
                }

            }



        });

    }

    private class CameraClickListener implements OnClickListener {

        private Command cmd;

        public CameraClickListener(Command cmd) {
            this.cmd = cmd;
        }

        @Override
        public void onClick(View p1) {
            // TODO: Implement this method
            if (!client.isStarted) {
                client.sendCmd(cmd);
            } else {
                toast.setText("追踪模式不可用");
                toast.show();
            }
        }


    }


}
