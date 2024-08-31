import os
import unittest
from src.main import merge_directories

class TestMergeDirectories(unittest.TestCase):
    def setUp(self):
        os.makedirs('test_source1', exist_ok=True)
        os.makedirs('test_source2', exist_ok=True)
        os.makedirs('test_target', exist_ok=True)
        with open('test_source1/file1.txt', 'w') as f:
            f.write('Hello')
        with open('test_source2/file2.txt', 'w') as f:
            f.write('World')

    def tearDown(self):
        shutil.rmtree('test_source1')
        shutil.rmtree('test_source2')
        shutil.rmtree('test_target')

    def test_merge_directories(self):
        merge_directories(['test_source1', 'test_source2'], 'test_target')
        self.assertTrue(os.path.exists('test_target/file1.txt'))
        self.assertTrue(os.path.exists('test_target/file2.txt'))

if __name__ == '__main__':
    unittest.main()