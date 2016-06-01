/**
 * Created by liuyang on 16-5-31.
 */
var blobSlice = File.prototype.slice || File.prototype.mozSlice || File.prototype.webkitSlice,
    input = document.getElementById('upload_file'),
    running = false,
    ua = navigator.userAgent.toLowerCase();

function calculateMD5(){
    if (running){
        return;
    }

    var fileReader = new FileReader(),
        file = input.files[0],
        time;
    
    $('#upload_name').val(file.name);

    fileReader.onload = function (e){
        running = false;

        if(file.size != e.target.result.byteLength){
            alert("Error: Browser reported success but could not read the file until the end.")
        } else {
            $('#upload_md5').val((SparkMD5.ArrayBuffer.hash(e.target.result)));
        }

    };
    running = true;
    $('#upload_md5').val("计算中...");
    time = new Date().getTime();
    fileReader.readAsArrayBuffer(file);
}

function newFolder(path){
    var newFolderName = $('#newFolderName').val();
    var current = window.location.href;
    window.location = '/newFolder?path=' + path + '&name=' + newFolderName + '&next=' + current 
}
    
