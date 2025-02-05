class ConfigManager:
    def __init__(self, **kwargs):
        self._config = kwargs

        for key, value in kwargs.items():
            setattr(self, key, value)
    
    def get(self, key, fallback=None):
        """returns a value of a key from Config._config"""
        return self._config.get(key, fallback)
    
    def set(self, key, value):
        """sets a value of a key to Config._config"""

        self._config[key] = value
        setattr(self, key, value) 