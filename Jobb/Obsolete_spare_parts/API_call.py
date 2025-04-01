import requests

# Vault server and authentication details
base_url = 'http://<vault-server-url>/AutodeskDM/Services/Vault/VaultServer'  # Replace with your Vault URL
username = 'your_username'
password = 'your_password'
auth_endpoint = f"{base_url}/Auth/Login"

# Authentication request
auth_data = {
    'username': username,
    'password': password,
    'authType': 'Windows'  # or 'Vault' based on your setup
}
response = requests.post(auth_endpoint, data=auth_data)

if response.status_code == 200:
    auth_token = response.json().get('sessionId')
else:
    raise Exception("Failed to authenticate with Vault")

# Search for the file '831420.iam' by file name
search_endpoint = f"{base_url}/Search/FindFilesBySearchConditions"
search_params = {
    'fileName': '831420.iam',  # File name to search
    'sessionId': auth_token
}
search_response = requests.get(search_endpoint, params=search_params)

if search_response.status_code == 200 and search_response.json():
    file_info = search_response.json()[0]  # Assuming the first result is the target file
    file_id = file_info.get('fileId')

    # Retrieve user-defined properties for the file
    metadata_endpoint = f"{base_url}/Properties/GetPropertyValues"
    metadata_params = {
        'fileId': file_id,
        'sessionId': auth_token
    }
    metadata_response = requests.get(metadata_endpoint, params=metadata_params)

    if metadata_response.status_code == 200:
        properties = metadata_response.json()
        title_description = next((prop['value'] for prop in properties if prop['name'] == 'Title Swe-Eng-Description'), None)
        print(f"Title Swe-Eng-Description: {title_description}")

    # Retrieve components for the file
    components_endpoint = f"{base_url}/File/GetComponents"
    components_params = {
        'fileId': file_id,
        'sessionId': auth_token
    }
    components_response = requests.get(components_endpoint, params=components_params)

    if components_response.status_code == 200:
        components = components_response.json()
        component_names = [comp['name'] for comp in components]
        print("Component Names:")
        for name in component_names:
            print(name)
    else:
        print("Failed to retrieve components")
else:
    print("File not found or search failed")
