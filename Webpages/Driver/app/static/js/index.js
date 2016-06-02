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
    

function copyto(src, path){
    var framediv = $('<div/>',{
        id: 'iframediv',
        style: 'position: absolute; top: 50%; left: 50%; width:400px; height: 300px; margin-top: -150px; margin-left: -200px'
    });
    var close = "<span class='glyphicon glyphicon-remove'></span><a href='javascript:void(0)' onclick='closeiframe()'>关闭</a>"
    var frame = $('<iframe/>',{
        src: '/copy?src='+src+'&srcpath='+path,
        width: '400',
        height: '300',
        display: 'block',
        frameborder: '0'
    });
    $('body').append(framediv);
    framediv.append(close);
    framediv.append(frame);
}

function closeiframe(){
    $('#iframediv').remove()
}

function moveto(src, path){
    var framediv = $('<div/>',{
        id: 'iframediv',
        style: 'position: absolute; top: 50%; left: 50%; width:400px; height: 300px; margin-top: -150px; margin-left: -200px'
    });
    var close = "<span class='glyphicon glyphicon-remove'></span><a href='javascript:void(0)' onclick='closeiframe()'>关闭</a>"
    var frame = $('<iframe/>',{
        src: '/move?src='+src+"&srcpath="+path,
        width: '400',
        height: '300',
        display: 'block',
        frameborder: '0'
    });
    $('body').append(framediv);
    framediv.append(close);
    framediv.append(frame);
}
