import streamlit as st
import pyrebase
import plotly.graph_objects as go 
from streamlit_toggle import st_toggle_switch
import time

st.set_page_config(page_title="Kipas Otomatis", page_icon="💨", layout='wide')

# Konfigurasi Firebase
config = {
    "apiKey": "your-api-key",
    "authDomain": "your-auth-domain",
    "databaseURL": "https://coba-53d06-default-rtdb.asia-southeast1.firebasedatabase.app/",
    "storageBucket": "coba-53d06.appspot.com",
    "serviceAccount": "serviceAccountKey.json"
}

firebase = pyrebase.initialize_app(config)
db = firebase.database()

st.markdown("""

    <style>
        .green-light {
            background-color: green;
            height: 100px;
            width: 100px;
            border-radius: 50%;
            justify-self: flex-end;
            
        }
        
        .red-light {
            background-color: red;
            height: 100px;
            width: 100px;
            border-radius: 50%;
            justify-self: flex-end;
        }
        
        .MuiGrid-justify-content-xs-flex-end {
            justify-content: flex-start;
        }
        
        .header {
            display: flex;
            flex-direction: column;
            background-color: #005EB8; 
            padding: 20px; 
            border-radius: 10px;  
            justify-content: center;  
        }
        
        .header h1, .header h3 {
            text-align: center;
        }
        
        .header h1 {
            font-size: 3.5rem;
        }
        
        .header h3 {
            background-color: skyblue;
        }
    </style>
    
    <div class="header">
        <h1>Fan Monitoring and Controling</h1>
        <h3>Salman Alfarizi Ramadhan - 22518241052</h3>
    </div>
    """, unsafe_allow_html=True)

def create_gauge(value, suhu, max, color):  

    fig = go.Figure(go.Indicator(
        mode="gauge+number",
        value=value,
        title={'text': suhu},
        gauge={
            'axis': {'range': [0, max]},  
            'bar': {'color': color},
        }
    ))
    return fig

def switch():
    # Cek apakah session state untuk switch sudah ada
    if "is_active" not in st.session_state:
        st.session_state.is_active = False  # Nilai default

    # Membuat switch dengan toggle
    st.session_state.is_active = st_toggle_switch(
        label="ON/OFF Kipas",
        key="key1",
        default_value=st.session_state.is_active,  # Ambil nilai dari session state
        label_after=False,  # Label sebelum/sesudah switch
    )

    # Menampilkan status switch
    if st.session_state.is_active:
        db.child("kipas").update({"switch":True})
    else:
        
        db.child("kipas").update({"switch":False})

def led():
    led = db.child("kipas").get().val().get("status")

    if led:
        st.markdown(
            "<div class='green-light'></div>",
            unsafe_allow_html=True
        )
    else:
        st.markdown(
            "<div class='red-light'></div>",
            unsafe_allow_html=True
        )

col1, col2 = st.columns(2, gap="small")

with st.container():
    with col1:
        temp_container = st.empty()
        
        switch()

        led_container = st.empty()

    with col2:    
        humid_container = st.empty()
    
while True:
    sensor = db.child("kipas").get().val()

    suhu = sensor.get("temp")
    humid = sensor.get("humid")

    with temp_container:
        fig = create_gauge(suhu, "Temperature (C)", 50, "blue")
        st.plotly_chart(fig, use_container_width=True) 

    with humid_container:
        fig = create_gauge(humid, "Humidity (%)", 100, "skyblue")
        st.plotly_chart(fig, use_container_width=True) 
    
    with led_container: 
        led()

    time.sleep(0.5)
