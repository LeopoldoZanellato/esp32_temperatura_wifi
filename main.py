import datetime
import pandas as pd
import gspread
import warnings
import requests
from oauth2client.service_account import ServiceAccountCredentials
import time

# Ignora avisos de depreciação específicos
warnings.filterwarnings("ignore", category=DeprecationWarning)


def get_google_sheet_connection(sheet_name, sheet_url):
    """
    Estabelece uma conexão com uma planilha do Google Sheets.

    :param sheet_name: Nome da aba da planilha
    :param sheet_url: URL da planilha do Google Sheets
    :return: Objeto worksheet para interação com a planilha
    """
    # Define o escopo de acesso
    scope = ["https://www.googleapis.com/auth/drive", 
             "https://www.googleapis.com/auth/spreadsheets",
             "https://spreadsheets.google.com/feeds"]

    # Substitua com a chave privada do arquivo de credencial
    credentials_dict = {
        "google credentials json"
    }
    credentials = ServiceAccountCredentials.from_json_keyfile_dict(credentials_dict, scope)

    # Autenticação e abertura da planilha
    client = gspread.authorize(credentials)
    sheet = client.open_by_url(sheet_url)
    worksheet = sheet.worksheet(sheet_name)
    return worksheet


def get_sheet_values(worksheet, range_to_get):
    """
    Obtém os valores de um intervalo específico da planilha.

    :param worksheet: Objeto worksheet da planilha
    :param range_to_get: Intervalo de células para obter os dados
    :return: DataFrame com os dados do intervalo especificado
    """
    values = worksheet.get(range_to_get)
    df = pd.DataFrame(values)
    df.columns = df.iloc[0]
    df.drop(axis=1, index=0, inplace=True)
    return df


def make_request_to_sensor(url):
    """
    Faz uma requisição HTTP a um sensor e retorna os dados.

    :param url: URL do sensor
    :return: Texto da resposta ou '0' se falhar
    """
    try:
        response = requests.get(url)
        return response.text if response.status_code == 200 else '0'
    except Exception as e:
        print(f"Erro desconhecido: {e}")
        return '0'


def main():
    """
    Função principal que executa o programa.
    """
    sensor_url = 'http://192.168.15.99'
    sensor_data = make_request_to_sensor(sensor_url)

    if sensor_data != '0':
        temperature, humidity = sensor_data.split(",")[:2]
        print(f"Temperatura {temperature}, umidade: {humidity}")
    else:
        temperature = humidity = '0'

    # Obtém os dados da planilha do Google Sheets
    google_sheet = get_google_sheet_connection("bd", "https://docs.google.com/spreadsheets/d/xxx")
    sheet_values = get_sheet_values(google_sheet, "A:E")
    sheet_values = sheet_values.fillna("")
    sheet_values = sheet_values[sheet_values['temperatura'] != '']

    # Atualiza a planilha com os novos dados
    last_row = str(len(sheet_values) + 2)
    current_time = datetime.datetime.now().strftime("%d/%m/%Y %H:%M:%S")

    if temperature != '0':
        google_sheet.update(f"A{last_row}", [[(str(temperature)).replace(".", ","), (str(humidity)).replace(".", ","), current_time]])
        print("realizado o update")
    else:
        print("erro")


if __name__ == "__main__":
    while True:
        try:
            main()
            time.sleep(300)  # Pausa o script por 300 segundos (5 minutos)
        except:
            time.sleep(300)  # Pausa o script por 300 segundos (5 minutos)
            pass